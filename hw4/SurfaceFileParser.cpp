#include "stdafx.h"
#include "SurfaceFileParser.h"
#include <regex>
#include <string>
#include <iosfwd>
#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>

//-----------------------------------------------------------------------------
SurfaceFileParser::SurfaceFileParser(void)
: m_current_line(-1)
, m_found_u_knots(false)
, m_found_v_knots(false)
, m_found_points(false)
, m_exp_knot_count_u(0)
, m_exp_knot_count_v(0)
, m_exp_pts_u(0)
, m_exp_pts_v(0)
, m_valid(false)
{
}

//-----------------------------------------------------------------------------
SurfaceFileParser::~SurfaceFileParser(void)
{
}

//-----------------------------------------------------------------------------
void SurfaceFileParser::clear()
{
	m_current_line		= -1;
	m_found_u_knots		= false;
	m_found_v_knots		= false;
	m_found_points		= false;
	m_exp_knot_count_u	= 0;
	m_exp_knot_count_v	= 0;
	m_exp_pts_u			= 0;
	m_exp_pts_v			= 0;
	m_valid				= false;

	m_temp_surf.Clear();
}

//-----------------------------------------------------------------------------
bool SurfaceFileParser::parse_file(const std::string& filename)
{
	clear();

	std::ifstream fs(filename.c_str());
	if(!fs)
	{
		return false;
	}

	// read into a vector
	std::string fileLine;
	char file_line[1000];
	while(!fs.eof())
	{
		fs.getline(file_line, 1000);
		fileLine = std::string(file_line);
		m_lines.push_back(fileLine);
	}

	// finish with the file.
	fs.close();

	// reset the temporary surface
	m_temp_surf.Clear();
	// remove all lines we don't need (blank and comments)
	trim_and_remove_unparsable();
	// reset the line counter
	m_current_line = 0;

	if(!check_order())
	{ return false; }

	if(!parse_order())
	{ return false; }

	// knots can appear in either order or not at all. {u}, {v}, {u,v}, {v,u}, {0}
	while(!m_found_points)
	{
		if(check_points())
		{ 
			m_found_points = true;
			break;
		}

		if(m_found_u_knots && m_found_v_knots)
		{ return false; }

		// if we haven't found u_knots yet, 
		if(!m_found_u_knots)
		{
			// try that here
			if(check_knots_u())
			{
				if(!parse_knots_u())
				{ return false; }

				m_found_u_knots = true;
				continue;
			}
		}

		// if we haven't found v_knots yet, 
		if(!m_found_v_knots)
		{
			// try that here, and advance if succeeded
			if(check_knots_v())
			{
				if(!parse_knots_v())
				{ return false; }

				m_found_v_knots = true;
				continue;
			}
		}
	}

	if(!check_points())
	{ return false; }

	if(!parse_points())
	{ return false; }

	m_valid = true;
	return true;
}

bool SurfaceFileParser::trim_and_remove_unparsable()
{
	std::vector<std::string>::iterator it = m_lines.begin();
	std::vector<std::string>::iterator it2;
	while(it != m_lines.end())
	{
		// trim the line.
		*it = trim(*it);

		const std::string& line = *it;
		bool remove = false;

		// remove if line starts with # or if line is empty
		if(line.empty()) {
			remove = true;
			::OutputDebugString("\"\"\n");
		} else if(line[0]=='#')  {
			remove = true;
			::OutputDebugString("#\n");
		}

		if(remove) 
		{
			// remove. hold an iterator pointing to the previous location
			it2 = it;
			bool start_over = false;
			if(it == m_lines.begin()) {
				start_over = true;
			} else {
				--it2;
			}
			// erase the current location
			m_lines.erase(it);
			// replace with temporary iterator
			it = start_over ? m_lines.begin() : it2;
		}
		else
		{
			++it;
		}
	}
	return true;
}

std::string SurfaceFileParser::trim(const std::string& line) const
{
	// trim the line first
	int firstNonWhitespace = line.find_first_not_of(" \t\n\r");
	int lastNonWhitespace = line.find_last_not_of(" \t\n\r");
	// if there's any bit of non-whitespace
	if(-1 != firstNonWhitespace)  {
		// replace with a trimmed line
		return line.substr(firstNonWhitespace, (lastNonWhitespace+1) - firstNonWhitespace);
	} else {
		// otherwise blank it out
		return "";
	}

}

bool SurfaceFileParser::chomp()
{
	return false;
}

bool SurfaceFileParser::check_order()
{
	const std::string& line = m_lines[m_current_line];
	const std::regex pattern("(\\d+)\\s+(\\d+)");
	return std::regex_match(line, pattern);
}

bool SurfaceFileParser::parse_order()
{
	const std::string& line = m_lines[m_current_line];
	const std::regex pattern("(\\d+)\\s+(\\d+)");
	std::match_results<std::string::const_iterator> results;
	if(!std::regex_match(line, results, pattern))
	{ return false; }

	assert(results.size() == 3);
	if(results.size() != 3)
	{ return false; }

	std::string order_u_str = results[1];
	std::string order_v_str = results[2];
	int order_u = atol(order_u_str.c_str());
	int order_v = atol(order_v_str.c_str());
	m_temp_surf.m_order.m_u = order_u;
	m_temp_surf.m_order.m_v = order_v;
	++m_current_line;
	return true;
}

bool SurfaceFileParser::check_knots_u()
{
	return _check_knots_uv(UVAxisU);
}

bool SurfaceFileParser::check_knots_v()
{
	return _check_knots_uv(UVAxisV);
}

bool SurfaceFileParser::_check_knots_uv(UVAxis axis)
{
	std::string axis_str;
	if(UVAxisU == axis){
		axis_str = "u";
	} else if(UVAxisV == axis){
		axis_str = "v";
	} else {
		return false;
	}

	const std::string& line = m_lines[m_current_line];
	// looks like u_knots[ N ]... or v_knots[ N ] ...
	std::string pattern_str = axis_str + std::string("_knots\\[\\s*\\d+\\s*\\].*");
	const std::regex pattern(pattern_str);
	return std::regex_match(line, pattern);
}

bool SurfaceFileParser::parse_knots_u()
{
	return _parse_knots_uv(UVAxisU);
}
bool SurfaceFileParser::parse_knots_v()
{
	return _parse_knots_uv(UVAxisV);
}

bool SurfaceFileParser::_parse_knots_uv(UVAxis axis)
{
	std::string axis_str;
	int* m_pexp_knot_count				= NULL;
	vector<double>* m_temp_surf_knot_vec	= NULL;
	
	if(UVAxisU == axis){
		axis_str = "u";
		m_pexp_knot_count				= &m_exp_knot_count_u;
		m_temp_surf_knot_vec			= &m_temp_surf.m_knots.m_u;
	} else if(UVAxisV == axis){
		axis_str = "v";
		m_pexp_knot_count				= &m_exp_knot_count_v;
		m_temp_surf_knot_vec			= &m_temp_surf.m_knots.m_v;
	} else {
		return false;
	}

	const std::string& line = m_lines[m_current_line];
	std::string pattern_str = axis_str + std::string("_knots\\[\\s*(\\d+)\\s*\\](.*)");
	const std::regex pattern(pattern_str);
	std::match_results<std::string::const_iterator> results;
	if(!std::regex_match(line, results, pattern))
	{ return false; }

	assert(1 <= results.size());
	if(1 > results.size())
	{ return false; }

	std::string knot_count_str = results[1];
	*m_pexp_knot_count = atol(knot_count_str.c_str());
	int exp_knot_count_u_tmp = *m_pexp_knot_count;

	// if there's anything next, it should be an equals sign
	std::string knots_line;
	std::string tail = results[2];
	tail = trim(tail);
	// if there's anything left on this line
	if(!tail.empty())
	{
		// look at that text
		knots_line = tail;
	} 
	else
	{
		// otherwise look after the = on the next line
		++m_current_line;
		knots_line = m_lines[m_current_line];
	}

	// there should be an equals sign in knots_line
	if(knots_line[0]!='=')
	{ return false; }

	// deal with equals. (it can be on its own line)
	knots_line = knots_line.substr(1);
	knots_line = trim(knots_line);
	// if there's nothing after it, jump to the next line
	if(knots_line.empty())
	{
		++m_current_line;
		if(m_lines.size() <= m_current_line)
		{ return false; } 

		knots_line = m_lines[m_current_line];
	}

	// now loop until we've found enough knots
	//bool done = false;
	//while(!done)
	//{
	//	std::stringstream sstrm(knots_line);
	//	while(!sstrm.eof() && !done)
	//	{
	//		double knot_val;
	//		bool success = sstrm >> knot_val;
	//		if(!success)
	//		{
	//			::OutputDebugString(std::string(std::string("Parse error at ") + sstrm.str()).c_str());
	//			return false;
	//		}

	//		m_temp_surf_knot_vec->push_back(knot_val);
	//		if(*m_pexp_knot_count == m_temp_surf_knot_vec->size())
	//		{
	//			done = true;
	//		}
	//	}
	//	++m_current_line;
	//	if(!done)
	//	{
	//		if(m_lines.size() <= m_current_line)
	//		{ return false; } 
	//		knots_line = m_lines[m_current_line];
	//	}
	//}


	// now loop until we've found enough knots
	int total_knots_exp = *m_pexp_knot_count;
	int curr_knots = 0;

	bool ended_early = false;
	std::stringstream sstrm(knots_line);
	for(int k=0; k<total_knots_exp; ++k)
	{
		double knot_val;
		bool success = sstrm >> knot_val;
		if(!success)
		{
			::OutputDebugString(std::string(std::string("Parse error at ") + sstrm.str()).c_str());
			return false;
		}
		m_temp_surf_knot_vec->push_back(knot_val);
		++curr_knots;

		if(sstrm.eof())
		{
			// advance to the next line
			++m_current_line;
			// if we're not done
			if(curr_knots < total_knots_exp)
			{
				// and we've reached the end of the string list, fail
				if(m_current_line >= m_lines.size())
				{ return false; }
				
				// otherwise, read the next line
				knots_line = m_lines[m_current_line];
				sstrm = std::stringstream(knots_line);
			}
			else
			{
				// if we are done, this should be the last iteration
				assert(total_knots_exp <= (k+1));
			}
		}
	}

	// if we didn't reach the end of the string, we probably didn't advance properly.
	if(!sstrm.eof())
	{
		++m_current_line;
	}
	return true;
}



bool SurfaceFileParser::check_points()
{
	if(m_lines.size() <= m_current_line)
	{ return false; }

	const std::string& line = m_lines[m_current_line];
	// looks like u_knots[ N ]... or v_knots[ N ] ...
	const std::regex pattern("points\\[\\s*\\d+\\s*\\].*");
	return std::regex_match(line, pattern);
}

bool SurfaceFileParser::parse_points()
{
	if(m_lines.size() <= m_current_line)
	{ return false; }

	const std::string& line = m_lines[m_current_line];
	const std::regex pattern("points\\[\\s*(\\d+)\\s*\\]\\[\\s*(\\d+)\\s*\\](.*)");
	std::match_results<std::string::const_iterator> results;
	if(!std::regex_match(line, results, pattern))
	{ return false; }

	assert(2 <= results.size());
	if(2 > results.size())
	{ return false; }

	std::string pt_count_str_u = results[1];
	m_exp_pts_u = atol(pt_count_str_u.c_str());
	int exp_pt_count_u_tmp = m_exp_pts_u;

	std::string pt_count_str_v = results[2];
	m_exp_pts_v = atol(pt_count_str_v.c_str());
	int exp_pt_count_v_tmp = m_exp_pts_v;

	// if there's anything next, it should be an equals sign
	std::string pts_line;
	std::string tail = results[3];
	tail = trim(tail);
	// if there's anything left on this line
	if(!tail.empty())
	{
		// look at that text
		pts_line = tail;
	} 
	else
	{
		// otherwise look after the = on the next line
		++m_current_line;
		pts_line = m_lines[m_current_line];
	}

	// there should be an equals sign in pts_line
	if(pts_line[0]!='=')
	{ return false; }

	// deal with equals. (it can be on its own line)
	pts_line = pts_line.substr(1);
	pts_line = trim(pts_line);
	// if there's nothing after it, jump to the next line
	if(pts_line.empty())
	{
		++m_current_line;
		if(m_lines.size() <= m_current_line)
		{ return false; } 

		pts_line = m_lines[m_current_line];
	}

	// finally, the loop where we acquire the points

	// now loop until we've found enough knots
	int total_points_exp = m_exp_pts_u * m_exp_pts_v;
	int curr_points = 0;

	std::vector<CCagdPoint> new_vec;
	for(int v=0; v<m_exp_pts_v; ++v)
	{
		m_temp_surf.m_points.push_back(new_vec);
		for(int u=0; u<m_exp_pts_u; ++u)
		{
			std::stringstream sstrm(pts_line);
			CCagdPoint pt3d;
			for(int i=0; i<3; ++i)
			{
				bool success = sstrm >> (&pt3d.x)[i];
				if(!success)
				{
					::OutputDebugString(std::string(std::string("Parse error at ") + sstrm.str()).c_str());
					return false;
				}
			}
			m_temp_surf.m_points[v].push_back(pt3d);
			++curr_points;
			++m_current_line;
			// if we're not done and we've reached the end of the string list fail.
			if(curr_points < (total_points_exp))
			{
				if(m_current_line >= m_lines.size())
				{ return false; }
				// otherwise grab the next line
				pts_line = m_lines[m_current_line];
			}				
		}
	}

	return true;
}
