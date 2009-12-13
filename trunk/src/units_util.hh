/*
 * mididings
 *
 * Copyright (C) 2008-2009  Dominic Sacré  <dominic.sacre@gmx.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef _UNITS_UTIL_HH
#define _UNITS_UTIL_HH

#include <cmath>
#include <algorithm>


enum VelocityMode {
    VELOCITY_MODE_OFFSET = 1,
    VELOCITY_MODE_MULTIPLY = 2,
    VELOCITY_MODE_FIXED = 3,
    VELOCITY_MODE_GAMMA = 4,
    VELOCITY_MODE_CURVE = 5,
};


enum ParameterIndices {
    PARAMETER_PORT = -1,
    PARAMETER_CHANNEL = -2,
    PARAMETER_DATA1 = -3,
    PARAMETER_DATA2 = -4,
};

#include <iostream>
inline int apply_velocity(int velocity, float value, VelocityMode mode)
{
    if (velocity == 0) {
        return 0;
    }

    switch (mode) {
      case VELOCITY_MODE_OFFSET:
        return velocity + (int)value;

      case VELOCITY_MODE_MULTIPLY:
        return (int)(velocity * value);

      case VELOCITY_MODE_FIXED:
        return (int)value;

      case VELOCITY_MODE_GAMMA:
        if (velocity > 0) {
            float a = (float)velocity / 127.f;
            float b = ::powf(a, 1.f / value);
            return std::max(1, (int)::rintf(b * 127.f));
        } else {
            return velocity;
        }

      case VELOCITY_MODE_CURVE:
        if (velocity > 0) {
            if (value != 0) {
                float p = -value;
                float a = ::expf(p * velocity / 127.f) - 1;
                float b = ::expf(p) - 1;
                return std::max(1, (int)(127.f * a / b));
            } else {
                return velocity;
            }
        } else {
            return 0;
        }

      default:
        return 0;
    }
}


/*
 * maps the input range [arg_lower ... arg_upper] to the
 * output range [val_lower ... val_upper]
 */
template <typename A, typename V>
V map_range(A arg, A arg_lower, A arg_upper, V val_lower, V val_upper)
{
    V value;

    if (arg <= arg_lower) {
        value = val_lower;
    } else if (arg >= arg_upper) {
        value = val_upper;
    } else {
        A dx = arg_upper - arg_lower;
        V dy = val_upper - val_lower;
        value = (V)((dy / dx) * (arg - arg_lower) + val_lower);
    }

    return value;
}


inline int get_parameter(int value, MidiEvent const & ev)
{
    if (value >= 0) {
        return value;
    }

    switch (value) {
      case PARAMETER_PORT:
        return ev.port;
      case PARAMETER_CHANNEL:
        return ev.channel;
      case PARAMETER_DATA1:
        return ev.data1;
      case PARAMETER_DATA2:
        return ev.data2;
      default:
        FAIL();
        return 0;
    }
}


#endif // _UNITS_UTIL_HH
