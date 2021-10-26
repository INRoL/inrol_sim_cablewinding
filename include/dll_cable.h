#define DLL_CABLE_API __declspec(dllexport)

class DLL_CABLE
{
public:
	static DLL_CABLE_API void set(int _N_link, double _r, double _L, double _rho, double _E, double _nu, double _mu, double _g, double _cyl_radius, double _cyl_center_x, double _cyl_center_y);
	static DLL_CABLE_API void forward(double p_des[3], double R_des[3][3]);
	static DLL_CABLE_API void get_info(double &time, double *data, double *FT);
};