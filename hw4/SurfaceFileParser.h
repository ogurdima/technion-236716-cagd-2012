#pragma once
#include <vector>
#include "cagd.h"
#include <string>

using std::vector;

class ParsedSurface
{
public:
	ParsedSurface() {}
	~ParsedSurface(){}
	void Clear() 
	{
		m_order.m_u = -1;
		m_order.m_v = -1;
		m_knots.m_u.clear();
		m_knots.m_v.clear();
		for(int i=0; i<m_points.size(); ++i)
		{
			m_points[i].clear();
		}
		m_points.clear();
	}

	// Order has u,v components
	struct Order
	{
		int m_u;
		int m_v;
	} m_order;

	// u and v knot vectors
	struct Knots
	{
		vector<double> m_u;
		vector<double> m_v;
	} m_knots;

	// control points
	vector<vector<CCagdPoint>> m_points;
};

enum UVAxis
{
	UVAxisU,
	UVAxisV
};


class SurfaceFileParser
{
	enum ParseState
	{
		ParseStateNone,
		ParseStateOrder,
		ParseStateKnots,
		ParseStatePoints,
		ParseStateDone
	};

public:
	SurfaceFileParser(void);
	~SurfaceFileParser(void);

	void clear();
	bool parse_file(const std::string& filename);
	bool trim_and_remove_unparsable();
	std::string trim(const std::string& line) const;

	// parse commands
	bool chomp();
	bool check_order();
	bool parse_order();
	bool check_knots_u();
	bool check_knots_v();
	bool _check_knots_uv(UVAxis axis);
	bool parse_knots_u();
	bool parse_knots_v();
	bool _parse_knots_uv(UVAxis axis);
	bool check_points();
	bool parse_points();

	// strip a line of whitespace at beginning and end
	bool trim_line(const std::string& line);

	std::vector<std::string> m_lines;
	int m_current_line;
	ParsedSurface m_temp_surf;

	bool m_found_u_knots;
	bool m_found_v_knots;
	bool m_found_points;
	int m_exp_knot_count_u;
	int m_exp_knot_count_v;
	int m_exp_pts_u;
	int m_exp_pts_v;
	bool m_valid;
};

