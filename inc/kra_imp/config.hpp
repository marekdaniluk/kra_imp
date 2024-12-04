/**
 * kraimp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */
#pragma once

#ifndef KRA_IMP_STATIC
	#ifdef _WIN32
		#ifdef kra_imp_EXPORTS
			#define KRA_IMP_API __declspec(dllexport)
		#else
			#define KRA_IMP_API __declspec(dllimport)
		#endif
	#elif __GNUC__ >= 4
		#define KRA_IMP_API __attribute__((visibility("default")))
	#else
		#define KRA_IMP_API
	#endif
#else
	#define KRA_IMP_API
#endif
