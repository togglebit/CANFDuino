/* ----------------------------------------------------------------------------
 *         SAM Software Package License
 * ----------------------------------------------------------------------------
 * Copyright (c) 2014, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following condition is met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */
#ifndef _SAM_INCLUDED_
#define _SAM_INCLUDED_

#define part_is_defined(part) (defined(__ ## part ## __))

/*
 * ----------------------------------------------------------------------------
 * SAMD51 family. These are not included in the SAMD or SAMD_SERIES defines
 * ----------------------------------------------------------------------------
 */

/* SAMD51 series */
#define SAMD51G18 ( \
    part_is_defined( SAMD51G18A ) )

#define SAMD51G19 ( \
    part_is_defined( SAMD51G19A ) )

#define SAMD51J18 ( \
    part_is_defined( SAMD51J18A ) )

#define SAMD51J19 ( \
    part_is_defined( SAMD51J19A ) )

#define SAMD51J20 ( \
    part_is_defined( SAMD51J20A ) )

#define SAMD51N19 ( \
    part_is_defined( SAMD51N19A ) )

#define SAMD51N20 ( \
    part_is_defined( SAMD51N20A ) )

#define SAMD51P19 ( \
    part_is_defined( SAMD51P19A ) )

#define SAMD51P20 ( \
    part_is_defined( SAMD51P20A ) )

/* Entire SAMD51G series */
#define SAMD51G_SERIES (SAMD51G18 || SAMD51G19)
#define SAMD51G SAMD51G_SERIES

/* Entire SAMD51J series */
#define SAMD51J_SERIES (SAMD51J18 || SAMD51J19 || SAMD51J20)
#define SAMD51J SAMD51J_SERIES

/* Entire SAMD51N series */
#define SAMD51N_SERIES (SAMD51N19 || SAMD51N20)
#define SAMD51N SAMD51N_SERIES

/* Entire SAMD51P series */
#define SAMD51P_SERIES (SAMD51P19 || SAMD51P20)
#define SAMD51P SAMD51P_SERIES

/* Entire SAMD51 series */
#define SAMD51_SERIES (SAMD51G18 || SAMD51G19 || SAMD51J18 || SAMD51J19 || SAMD51J20 || SAMD51N19 || SAMD51N20 || SAMD51P19 || SAMD51P20)
#define SAMD51  SAMD51_SERIES

/*
 * ----------------------------------------------------------------------------
 * SAML family
 * ----------------------------------------------------------------------------
 */

/* SAML21 series */
#define SAML21E15 ( \
    part_is_defined( SAML21E15B ) )

#define SAML21E16 ( \
    part_is_defined( SAML21E16B ) )

#define SAML21E17 ( \
    part_is_defined( SAML21E17B ) )

#define SAML21E18 ( \
    part_is_defined( SAML21E18B ) )

#define SAML21G16 ( \
    part_is_defined( SAML21G16B ) )

#define SAML21G17 ( \
    part_is_defined( SAML21G17B ) )

#define SAML21G18 ( \
    part_is_defined( SAML21G18B ) )

#define SAML21J16 ( \
    part_is_defined( SAML21J16B ) )

#define SAML21J17 ( \
    part_is_defined( SAML21J17B ) )

#define SAML21J18 ( \
    part_is_defined( SAML21J18B ) )

/* Entire SAML21E series */
#define SAML21E_SERIES (SAML21E15 || SAML21E16 || SAML21E17 || SAML21E18)
#define SAML21E	SAML21E_SERIES

/* Entire SAML21G series */
#define SAML21G_SERIES (SAML21G16 || SAML21G17 || SAML21G18)
#define SAML21G	SAML21G_SERIES

/* Entire SAML21J series */
#define SAML21J_SERIES (SAML21J16 || SAML21J17 || SAML21J18)
#define SAML21J	SAML21J_SERIES

/* Entire SAML21 series */
#define SAML21_SERIES (SAML21E15 || SAML21E16 || SAML21E17 || SAML21E18 || SAML21G16 || SAML21G17 || SAML21G18 || SAML21J16 || SAML21J17 || SAML21J18)
#define SAML21	SAML21_SERIES

/* Entire SAML family */
#define SAML_SERIES (SAML21_SERIES)
#define SAML	SAML_SERIES

/*
 * ----------------------------------------------------------------------------
 * SAMC family
 * ----------------------------------------------------------------------------
 */

/* SAMC21 series */
#define SAMC21E15 ( \
    part_is_defined( SAMC21E15A ) )

#define SAMC21E16 ( \
    part_is_defined( SAMC21E16A ) )

#define SAMC21E17 ( \
    part_is_defined( SAMC21E17A ) )

#define SAMC21E18 ( \
    part_is_defined( SAMC21E18A ) )

#define SAMC21G15 ( \
    part_is_defined( SAMC21G15A ) )

#define SAMC21G16 ( \
    part_is_defined( SAMC21G16A ) )

#define SAMC21G17 ( \
    part_is_defined( SAMC21G17A ) )

#define SAMC21G18 ( \
    part_is_defined( SAMC21G18A ) )

#define SAMC21J15 ( \
    part_is_defined( SAMC21J15A ) )

#define SAMC21J16 ( \
    part_is_defined( SAMC21J16A ) )

#define SAMC21J17 ( \
    part_is_defined( SAMC21J17A ) )

#define SAMC21J18 ( \
    part_is_defined( SAMC21J18A ) )

/* Entire SAMC21E series */
#define SAMC21E_SERIES (SAMC21E15 || SAMC21E16 || SAMC21E17 || SAMC21E18)
#define SAMC21E	SAMC21E_SERIES

/* Entire SAMC21G series */
#define SAMC21G_SERIES (SAMC21G15 || SAMC21G16 || SAMC21G17 || SAMC21G18)
#define SAMC21G	SAMC21G_SERIES

/* Entire SAMC21J series */
#define SAMC21J_SERIES (SAMC21J15 || SAMC21J16 || SAMC21J17 || SAMC21J18)
#define SAMC21J	SAMC21J_SERIES

/* Entire SAMC21 series */
#define SAMC21_SERIES (SAMC21E15 || SAMC21E16 || SAMC21E17 || SAMC21E18 || SAMC21G15 || SAMC21G16 || SAMC21G17 || SAMC21G18 || SAMC21J15 || SAMC21J16 || SAMC21J17 || SAMC21J18)
#define SAMC21	SAMC21_SERIES

/* Entire SAMC family */
#define SAMC_SERIES (SAMC21_SERIES)
#define SAMC SAMC_SERIES

/*
 * ----------------------------------------------------------------------------
 * SAMD family
 * ----------------------------------------------------------------------------
 */

/* SAMD11 series */
#define SAMD11C14 ( \
    part_is_defined( SAMD11C14A ) )
#define SAMD11C	SAMD11C14

#define SAMD11D14 ( \
    part_is_defined( SAMD11D14AS ) || \
    part_is_defined( SAMD11D14AM ) || \
    part_is_defined( SAMD11D14AU ) )
#define SAMD11D	SAMD11D14

/* Entire SAMD11 series */
#define SAMD11_SERIES (SAMD11C14 || SAMD11D14)
#define SAMD11	SAMD11_SERIES

/* SAMD21 series */
#define SAMD21E15 ( \
    part_is_defined( SAMD21E15A ) || \
    part_is_defined( SAMD21E15B ) || \
    part_is_defined( SAMD21E15BU ) || \
    part_is_defined( SAMD21E15L ) )

#define SAMD21E16 ( \
    part_is_defined( SAMD21E16A ) || \
    part_is_defined( SAMD21E16B ) || \
    part_is_defined( SAMD21E16BU ) || \
    part_is_defined( SAMD21E16L ) )

#define SAMD21E17 ( \
    part_is_defined( SAMD21E17A ) )

#define SAMD21E18 ( \
    part_is_defined( SAMD21E18A ) )

#define SAMD21G15 ( \
    part_is_defined( SAMD21G15A ) || \
    part_is_defined( SAMD21G15B ) || \
    part_is_defined( SAMD21G15L ) )

#define SAMD21G16 ( \
    part_is_defined( SAMD21G16A ) || \
    part_is_defined( SAMD21G16B ) || \
    part_is_defined( SAMD21G16L ) )

#define SAMD21G17 ( \
    part_is_defined( SAMD21G17A ) || \
    part_is_defined( SAMD21G17AU ) )

#define SAMD21G18 ( \
    part_is_defined( SAMD21G18A ) || \
    part_is_defined( SAMD21G18AU ) )

#define SAMD21J15 ( \
    part_is_defined( SAMD21J15A ) || \
    part_is_defined( SAMD21J15B ) )

#define SAMD21J16 ( \
    part_is_defined( SAMD21J16A ) || \
    part_is_defined( SAMD21J16B ) )

#define SAMD21J17 ( \
    part_is_defined( SAMD21J17A ) )

#define SAMD21J18 ( \
    part_is_defined( SAMD21J18A ) )

/* Entire SAMD21E series */
#define SAMD21E_SERIES (SAMD21E15 || SAMD21E16 || SAMD21E17 || SAMD21E18)
#define SAMD21E	SAMD21E_SERIES

/* Entire SAMD21G series */
#define SAMD21G_SERIES (SAMD21G15 || SAMD21G16 || SAMD21G17 || SAMD21G18)
#define SAMD21G	SAMD21G_SERIES

/* Entire SAMD21J series */
#define SAMD21J_SERIES (SAMD21J15 || SAMD21J16 || SAMD21J17 || SAMD21J18)
#define SAMD21J	SAMD21J_SERIES

/* Entire SAMD21 series */
#define SAMD21_SERIES (SAMD21E15 || SAMD21E16 || SAMD21E17 || SAMD21E18 || SAMD21G15 || SAMD21G16 || SAMD21G17 || SAMD21G18 || SAMD21J15 || SAMD21J16 || SAMD21J17 || SAMD21J18)
#define SAMD21	SAMD21_SERIES

/* Entire SAMD family */
#define SAMD_SERIES (SAMD11_SERIES || SAMD21_SERIES)
#define SAMD	SAMD_SERIES

/*
 * ----------------------------------------------------------------------------
 * Whole SAM product line
 * ----------------------------------------------------------------------------
 */

#define SAM (SAML_SERIES || SAMD_SERIES || SAMC_SERIES || SAMD51_SERIES)

/*
 * ----------------------------------------------------------------------------
 * Header inclusion
 * ----------------------------------------------------------------------------
 */

#if SAML_SERIES
#include "saml21/include/saml21.h"
#endif /* SAML_SERIES */

#if SAMC_SERIES
#include "samc21/include/samc21.h"
#endif /* SAMC_SERIES */

#if SAMD21_SERIES
#include "samd21/include/samd21.h"
#endif /* SAMD21_SERIES */

#if SAMD11_SERIES
#include "samd11/include/samd11.h"
#endif /* SAMD11_SERIES */

#if SAMD51_SERIES
#include "samd51/include/samd51.h"
#endif /* SAMD51_SERIES */

#endif
