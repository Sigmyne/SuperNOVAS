/**
 * @file
 *
 *  SuperNOVAS functions interfacing with the NAIF CSPICE Toolkit.
 *
 * @date Created  on Nov 12, 2024
 * @author Attila Kovacs
 */

#ifndef NOVAS_CSPICE_H_
#define NOVAS_CSPICE_H_

#if __cplusplus
extern "C" {
#endif

/// @ingroup solar-system
int novas_cspice_is_thread_safe();

/// @ingroup solar-system
int novas_use_cspice();

/// @ingroup solar-system
int novas_use_cspice_ephem();

/// @ingroup solar-system
int novas_use_cspice_planets();

/// @ingroup solar-system
int cspice_add_kernel(const char *filename);

/// @ingroup solar-system
int cspice_remove_kernel(const char *filename);

#if __cplusplus
} // extern "C"
#endif

#endif /* NOVAS_CSPICE_H_ */
