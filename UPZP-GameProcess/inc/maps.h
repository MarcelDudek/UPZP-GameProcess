#ifndef UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_INC_MAPS_H_
#define UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_INC_MAPS_H_

namespace upzp {

/**
 * List of possible maps.
 */
enum class Maps {
  WARSZAWA,
  WROCLAW,
  KRAKOW
};

/**
 * @brief Get map start point latitude.
 * @param map Selected map.
 * @return Latitude.
 */
constexpr double MapsLatitude(Maps map) {
  switch (map) {
    case Maps::WARSZAWA:
      return 21.025745;
    case Maps::WROCLAW:
      return 17.034299;
    case Maps::KRAKOW:
      return 19.938898;
    default:
      return 0.0;
  }
}

/**
 * @brief Get map start point longitude.
 * @param map Selected map.
 * @return Longitude.
 */
constexpr double MapsLongitude(Maps map) {
  switch (map) {
    case Maps::WARSZAWA:
      return 52.240679;
    case Maps::WROCLAW:
      return 51.109385;
    case Maps::KRAKOW:
      return 50.061974;
    default:
      return 0.0;
  }
}

}  // namespace upzp

#endif //UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_INC_MAPS_H_
