
		#include "levelmodel.h"
		//bullet
		#include <btBulletDynamicsCommon.h>
		#include "LinearMath/btHashMap.h"

		using namespace troen;

		//!!!!!!!!!!!!! WARNING: AUTO_GENERATED !!!!!!!!!!!!!!!!!!!!!!
		// If you want to change something generally, please edit obstacle_export.py, otherwise be sure to mark changes to this code otherwise it might be overwritten


		void LevelModel::auto_addObstacles()
		{
			// obstacles
			// TODO grab the value from origin
			std::vector<BoxModel> newObstacles = {

			
			{
				btVector3(-102.9119873046875, -2690.1763916015625,51.45334720611572),
				btVector3(5342.998046875, 75.45331954956055, 198.79688262939453),
				btQuaternion(0.0,0.0,5.960464477539063e-08,-0.9999999403953552),
                std::string("Cube.1278")
			},

			{
				btVector3(-57.116546630859375, 2625.8404541015625,57.73476600646973),
				btVector3(5356.66748046875, 75.45375347137451, 211.36653900146484),
				btQuaternion(0.0,0.0,5.960464477539063e-08,-0.9999999403953552),
                std::string("Cube.1277")
			},

			{
				btVector3(2612.587890625, -39.881746768951416,57.73476600646973),
				btVector3(5356.6680908203125, 75.45376300811768, 211.36653900146484),
				btQuaternion(0.0,0.0,0.7071067690849304,-0.7071067690849304),
                std::string("Cube.1276")
			},

			{
				btVector3(-2704.2196655273438, 5.9132981300354,51.45334720611572),
				btVector3(5342.999267578125, 75.45332908630371, 198.79688262939453),
				btQuaternion(0.0,0.0,0.7071067690849304,-0.7071067690849304),
                std::string("Cube.1245")
			},

			{
				btVector3(746.1251831054688, -637.6168060302734,292.9811477661133),
				btVector3(184.14979934692383, 184.14979934692383, 600.5661010742188),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.106")
			},

			{
				btVector3(1512.0306396484375, 1735.494384765625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.105")
			},

			{
				btVector3(1512.0306396484375, 1299.4976806640625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.104")
			},

			{
				btVector3(1512.0306396484375, 863.5012817382812,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.103")
			},

			{
				btVector3(1512.0306396484375, 427.5050354003906,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.102")
			},

			{
				btVector3(1512.0306396484375, -8.4912109375,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.101")
			},

			{
				btVector3(1512.0306396484375, -444.48760986328125,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.100")
			},

			{
				btVector3(1512.0306396484375, -880.4838562011719,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.099")
			},

			{
				btVector3(1512.0306396484375, -1316.4801025390625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.098")
			},

			{
				btVector3(1512.0306396484375, -1752.4765014648438,9.99596655368805),
				btVector3(20.0, 20.000009536743164, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.097")
			},

			{
				btVector3(1186.4251708984375, 2171.490478515625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.096")
			},

			{
				btVector3(1186.4251708984375, 1735.494384765625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.095")
			},

			{
				btVector3(1186.4251708984375, 1299.4976806640625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.094")
			},

			{
				btVector3(1186.4251708984375, 863.5012817382812,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.093")
			},

			{
				btVector3(1186.4251708984375, 427.5050354003906,7.228968143463135),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.092")
			},

			{
				btVector3(1186.4251708984375, -8.4912109375,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.091")
			},

			{
				btVector3(1186.4251708984375, -444.48760986328125,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.090")
			},

			{
				btVector3(1186.4251708984375, -880.4838562011719,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.089")
			},

			{
				btVector3(1186.4251708984375, -1316.4801025390625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.088")
			},

			{
				btVector3(1186.4251708984375, -1752.4765014648438,9.99596655368805),
				btVector3(20.0, 20.000009536743164, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.087")
			},

			{
				btVector3(860.8195495605469, 2171.490478515625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.086")
			},

			{
				btVector3(860.8195495605469, 1735.494384765625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.085")
			},

			{
				btVector3(860.8195495605469, 1299.4976806640625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.084")
			},

			{
				btVector3(860.8195495605469, 863.5012817382812,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.083")
			},

			{
				btVector3(860.8195495605469, 427.5050354003906,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.082")
			},

			{
				btVector3(860.8195495605469, -8.4912109375,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.081")
			},

			{
				btVector3(860.8195495605469, -444.48760986328125,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.080")
			},

			{
				btVector3(860.8195495605469, -880.4838562011719,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.079")
			},

			{
				btVector3(860.8195495605469, -1316.4801025390625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.078")
			},

			{
				btVector3(860.8195495605469, -1752.4765014648438,9.99596655368805),
				btVector3(20.0, 20.000009536743164, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.077")
			},

			{
				btVector3(535.2140808105469, 2171.490478515625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.076")
			},

			{
				btVector3(535.2140808105469, 1735.494384765625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.075")
			},

			{
				btVector3(535.2140808105469, 1299.4976806640625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.074")
			},

			{
				btVector3(535.2140808105469, 863.5012817382812,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.073")
			},

			{
				btVector3(535.2140808105469, 427.5050354003906,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.072")
			},

			{
				btVector3(752.2522735595703, 374.6894073486328,294.09372329711914),
				btVector3(184.14979934692383, 184.14979934692383, 600.5661010742188),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.071")
			},

			{
				btVector3(535.2140808105469, -444.48760986328125,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.070")
			},

			{
				btVector3(535.2140808105469, -880.4838562011719,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.069")
			},

			{
				btVector3(535.2140808105469, -1316.4801025390625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.068")
			},

			{
				btVector3(535.2140808105469, -1752.4765014648438,9.99596655368805),
				btVector3(20.0, 20.000009536743164, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.067")
			},

			{
				btVector3(209.60830688476562, 2171.490478515625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.066")
			},

			{
				btVector3(209.60830688476562, 1735.494384765625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.065")
			},

			{
				btVector3(209.60830688476562, 1299.4976806640625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.064")
			},

			{
				btVector3(209.60830688476562, 863.5012817382812,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.063")
			},

			{
				btVector3(209.60830688476562, 427.5050354003906,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.062")
			},

			{
				btVector3(94.06496047973633, -8.4912109375,10.0),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.061")
			},

			{
				btVector3(209.60830688476562, -444.48760986328125,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.060")
			},

			{
				btVector3(209.60830688476562, -880.4838562011719,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.059")
			},

			{
				btVector3(209.60830688476562, -1316.4801025390625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.058")
			},

			{
				btVector3(209.60830688476562, -1752.4765014648438,9.99596655368805),
				btVector3(20.0, 20.000009536743164, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.057")
			},

			{
				btVector3(-115.997314453125, 2171.490478515625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.056")
			},

			{
				btVector3(-115.997314453125, 1735.494384765625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.055")
			},

			{
				btVector3(-115.997314453125, 1299.4976806640625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.054")
			},

			{
				btVector3(-115.997314453125, 863.5012817382812,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.053")
			},

			{
				btVector3(-115.997314453125, 427.5050354003906,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.052")
			},

			{
				btVector3(-115.997314453125, -8.4912109375,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.051")
			},

			{
				btVector3(-115.997314453125, -444.48760986328125,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.050")
			},

			{
				btVector3(-115.997314453125, -880.4838562011719,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.049")
			},

			{
				btVector3(-115.997314453125, -1316.4801025390625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.048")
			},

			{
				btVector3(-115.997314453125, -1752.4765014648438,9.99596655368805),
				btVector3(20.0, 20.000009536743164, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.047")
			},

			{
				btVector3(-441.6028594970703, 2171.490478515625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.046")
			},

			{
				btVector3(-441.6028594970703, 1735.494384765625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.045")
			},

			{
				btVector3(-441.6028594970703, 1299.4976806640625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.044")
			},

			{
				btVector3(-441.6028594970703, 863.5012817382812,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.043")
			},

			{
				btVector3(-441.6028594970703, 427.5050354003906,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.042")
			},

			{
				btVector3(-441.6028594970703, -8.4912109375,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.041")
			},

			{
				btVector3(-441.6028594970703, -444.48760986328125,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.040")
			},

			{
				btVector3(-441.6028594970703, -880.4838562011719,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.039")
			},

			{
				btVector3(-441.6028594970703, -1316.4801025390625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.038")
			},

			{
				btVector3(-441.6028594970703, -1752.4765014648438,9.99596655368805),
				btVector3(20.0, 20.000009536743164, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.037")
			},

			{
				btVector3(-767.2085571289062, 2171.490478515625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.036")
			},

			{
				btVector3(-767.2085571289062, 1735.494384765625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.035")
			},

			{
				btVector3(-767.2085571289062, 1299.4976806640625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.034")
			},

			{
				btVector3(-767.2085571289062, 863.5012817382812,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.033")
			},

			{
				btVector3(-767.2085571289062, 427.5050354003906,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.032")
			},

			{
				btVector3(-767.2085571289062, -8.4912109375,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.031")
			},

			{
				btVector3(-767.2085571289062, -444.48760986328125,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.030")
			},

			{
				btVector3(-767.2085571289062, -880.4838562011719,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.029")
			},

			{
				btVector3(-767.2085571289062, -1316.4801025390625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.028")
			},

			{
				btVector3(-767.2085571289062, -1752.4765014648438,9.99596655368805),
				btVector3(20.0, 20.000009536743164, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.027")
			},

			{
				btVector3(-1092.8141784667969, 2171.490478515625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.026")
			},

			{
				btVector3(-1092.8141784667969, 1735.494384765625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.025")
			},

			{
				btVector3(-1092.8141784667969, 1299.4976806640625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.024")
			},

			{
				btVector3(-1092.8141784667969, 863.5012817382812,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.023")
			},

			{
				btVector3(-1092.8141784667969, 427.5050354003906,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.022")
			},

			{
				btVector3(-1092.8141784667969, -8.4912109375,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.021")
			},

			{
				btVector3(-1092.8141784667969, -444.48760986328125,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.020")
			},

			{
				btVector3(-1092.8141784667969, -880.4838562011719,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.019")
			},

			{
				btVector3(-1092.8141784667969, -1316.4801025390625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.018")
			},

			{
				btVector3(-1092.8141784667969, -1752.4765014648438,9.99596655368805),
				btVector3(20.0, 20.000009536743164, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.017")
			},

			{
				btVector3(-1418.4197998046875, 2171.490478515625,9.99596655368805),
				btVector3(20.000009536743164, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.016")
			},

			{
				btVector3(-1418.4197998046875, 1735.494384765625,9.99596655368805),
				btVector3(20.000009536743164, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.015")
			},

			{
				btVector3(-1418.4197998046875, 1299.4976806640625,9.99596655368805),
				btVector3(20.000009536743164, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.014")
			},

			{
				btVector3(-1418.4197998046875, 863.5012817382812,9.99596655368805),
				btVector3(20.000009536743164, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.013")
			},

			{
				btVector3(-1418.4197998046875, 427.5050354003906,9.99596655368805),
				btVector3(20.000009536743164, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.012")
			},

			{
				btVector3(-1418.4197998046875, -8.4912109375,9.99596655368805),
				btVector3(20.000009536743164, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.011")
			},

			{
				btVector3(-1418.4197998046875, -444.48760986328125,9.99596655368805),
				btVector3(20.000009536743164, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.010")
			},

			{
				btVector3(-1418.4197998046875, -880.4838562011719,9.99596655368805),
				btVector3(20.000009536743164, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.009")
			},

			{
				btVector3(-1418.4197998046875, -1316.4801025390625,9.99596655368805),
				btVector3(20.000009536743164, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.008")
			},

			{
				btVector3(-1418.4197998046875, -1752.4765014648438,9.99596655368805),
				btVector3(20.000009536743164, 20.000009536743164, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.007")
			},

			{
				btVector3(342.8825378417969, 918.4856414794922,152.43303298950195),
				btVector3(1020.994873046875, 898.2110595703125, 6.920419335365295),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.006")
			},

			{
				btVector3(568.7586212158203, -879.2779541015625,146.33319854736328),
				btVector3(683.0246734619141, 321.9303894042969, 19.53296184539795),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.004")
			},

			{
				btVector3(988.9772033691406, 162.55023956298828,150.13311386108398),
				btVector3(288.5529136657715, 2405.4783630371094, 11.89049243927002),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.003")
			},

			{
				btVector3(-10.782455205917358, -778.228759765625,151.9006633758545),
				btVector3(531.1703491210938, 531.1703491210938, 8.43999981880188),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube.002")
			},

			{
				btVector3(-10.268763303756714, -301.5158462524414,72.03860759735107),
				btVector3(529.6404647827148, 461.7601776123047, 12.254399061203003),
				btQuaternion(-4.367650063841211e-08,-0.9845066666603088,0.17534898221492767,-1.7463129831085666e-09),
                std::string("Cube.001")
			},

			{
				btVector3(1512.0306396484375, 2171.490478515625,9.99596655368805),
				btVector3(20.0, 20.0, 20.0),
				btQuaternion(0.0,0.0,0.0,1.0),
                std::string("Cube")
			} 

			};
			m_obstacles.insert(m_obstacles.end(), newObstacles.begin(), newObstacles.end());
		
			addBoxes(m_obstacles, LEVELOBSTACLETYPE);
		}
	