/// <summary>
/// Holds common definitions
/// </summary>
class Definitions {
public:
	constexpr static double MAX_ZOOM = 1000 / 100.0; // defined in terms of percentage. In this case the max zoom is 1000%
	constexpr static double MIN_ZOOM = 15 / 100.0;
	
	constexpr static double MAX_BRIGHTNESS = 100;
	constexpr static double MIN_BRIGHTNESS = -100;

	constexpr static double MAX_CONTRAST = 128;
	constexpr static double MIN_CONTRAST = -128;
};