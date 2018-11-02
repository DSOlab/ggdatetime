///
/// @file  gnsstm.hpp
///
/// @brief Time-Scales used in GNSS
///

#ifndef __GNSS_TIME_SCALES_HPP__
#define __GNSS_TIME_SCALES_HPP__

namespace ngpt
{

/// @enum GnssTimeSystem
/// Time-Systems used in GNSS, see Rinex v3.xx
/// Apart from the small errors in the realizations of the different time 
/// systems, the relations between the systems are: 
/// GLO = UTC = GPS - Δt_ls
/// GPS = GAL = UTC + Δt_ls
/// GPS = QZS = UTC + Δt_ls
/// GPS = IRN = UTC + Δt_ls
/// BDT =     = UTC + Δt_lsBDS
/// where Δt_ls is delta time between GPS and UTC due to leap seconds, as
/// transmitted by the GPS satellites in the almanac.
/// Δt_lsBDS is is delta time between BDT and UTC due to leap seconds, as
/// transmitted by the BDT satellites in the almanac.
enum class GnssTimeSystem
: char
{
    gps, ///< to identify GPS time
    glo, ///< to identify the GLONASS UTC time system 
    gal, ///< to identify Galileo time
    qzs, ///< to identify QZSS time
    bdt, ///< to identify BDS time
    irn  ///< to identify IRNSS time
};// GnssTimeSystem

}// namespace ngpt

#endif
