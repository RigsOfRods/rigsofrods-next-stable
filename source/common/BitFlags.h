#pragma once

#include <OgrePlatform.h>

/** @file Bit operations */

// BITMASK(1) = 0x00000001 = 0b00....0001
// BITMASK(2) = 0x00000002 = 0b00....0010
#define BITMASK( OFFSET )           (1 << ( (OFFSET) - 1))

#define BITMASK_SET_0( VAR, FLAGS ) ( (VAR) &= ~ (FLAGS) )

#define BITMASK_SET_1( VAR, FLAGS ) ( (VAR) |= (FLAGS) )

template<typename var_T, typename flags_T> void Bitmask_SetBool(bool & val, var_T var, flags_T flags)
{
	var_T var_true  = BITMASK_SET_1(var, flags);
	var_T var_false = BITMASK_SET_0(var, flags);
	var = (val) ? var_true : var_false;
}

#define BITMASK_IS_0( VAR, FLAGS )  ( ((VAR) & (FLAGS)) == 0 )

#define BITMASK_IS_1( VAR, FLAGS )  ( ((VAR) & (FLAGS)) == (FLAGS) )

// --------------------------------------------------------------------------------

#define BITMASK_64( OFFSET )           ( Ogre::uint64(1) << ((OFFSET) - 1) )

#define BITMASK_64_SET_0               BITMASK_SET_0

#define BITMASK_64_SET_1               BITMASK_SET_1

#define BITMASK_64_IS_0( VAR, FLAGS )  ( ((VAR) & (FLAGS)) == Ogre::uint64(0) )

#define BITMASK_64_IS_1                BITMASK_IS_1