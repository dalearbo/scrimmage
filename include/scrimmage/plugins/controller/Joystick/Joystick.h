/*!
 * @file
 *
 * @section LICENSE
 *
 * Copyright (C) 2017 by the Georgia Tech Research Institute (GTRI)
 *
 * This file is part of SCRIMMAGE.
 *
 *   SCRIMMAGE is free software: you can redistribute it and/or modify it under
 *   the terms of the GNU Lesser General Public License as published by the
 *   Free Software Foundation, either version 3 of the License, or (at your
 *   option) any later version.
 *
 *   SCRIMMAGE is distributed in the hope that it will be useful, but WITHOUT
 *   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 *   License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with SCRIMMAGE.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Kevin DeMarco <kevin.demarco@gtri.gatech.edu>
 * @author Eric Squires <eric.squires@gtri.gatech.edu>
 * @date 31 July 2017
 * @version 0.1.0
 * @brief Brief file description.
 * @section DESCRIPTION
 * A Long description goes here.
 *
 */

#ifndef INCLUDE_SCRIMMAGE_PLUGINS_CONTROLLER_JOYSTICK_JOYSTICK_H_
#define INCLUDE_SCRIMMAGE_PLUGINS_CONTROLLER_JOYSTICK_JOYSTICK_H_

#include <scrimmage/motion/Controller.h>

#include <Eigen/Dense>

#include <map>
#include <string>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

namespace scrimmage {
namespace controller {

class Joystick : public scrimmage::Controller {
 public:
    Joystick();
    virtual void init(std::map<std::string, std::string> &params);
    virtual bool step(double t, double dt);

 protected:
    int joy_fd_;
    int *axis_ = NULL;
    int num_of_axis_ = 0;
    int num_of_buttons_ = 0;
	char *button_ = NULL;
    char *name_of_joystick_[80];
	struct js_event js_;

    int min_value = -32767;
    int max_value = +32767;

    bool print_js_values_ = false;
};
} // namespace controller
} // namespace scrimmage
#endif // INCLUDE_SCRIMMAGE_PLUGINS_CONTROLLER_JOYSTICK_JOYSTICK_H_
