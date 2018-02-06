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

#include <scrimmage/plugins/controller/JoystickController/JoystickController.h>

#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include <scrimmage/entity/Entity.h>
#include <scrimmage/math/State.h>
#include <scrimmage/common/Utilities.h>
#include <scrimmage/parse/ParseUtils.h>

#include <iostream>
#include <limits>

using std::cout;
using std::endl;

namespace sc = scrimmage;

REGISTER_PLUGIN(scrimmage::Controller,
                scrimmage::controller::JoystickController,
                JoystickController_plugin)

namespace scrimmage {
namespace controller {

JoystickController::JoystickController() {
}

JoystickController::~JoystickController() {
    free(axis_);
    free(button_);
}

void JoystickController::init(std::map<std::string, std::string> &params) {

    print_js_values_ = sc::get<bool>("print_raw_joystick_values", params, false);

    if ((joy_fd_ = open("/dev/input/js0", O_RDONLY)) == -1) {
		cout << "couln't open joystick" << endl;
	}

    char name_of_joystick[80];

    ioctl(joy_fd_, JSIOCGAXES, &num_of_axis_);
	ioctl(joy_fd_, JSIOCGBUTTONS, &num_of_buttons_);
	ioctl(joy_fd_, JSIOCGNAME(80), &name_of_joystick);

	axis_ = reinterpret_cast<int*>(calloc(num_of_axis_, sizeof(int)));
	button_ = reinterpret_cast<char*>(calloc(num_of_buttons_, sizeof(char)));

    if (print_js_values_) {
        cout << "Joystick detected:" << *name_of_joystick << endl;
        cout << "\t " << num_of_axis_ << " axis" << endl;
        cout << "\t " << num_of_buttons_ << " buttons" << endl;
    }

	fcntl(joy_fd_, F_SETFL, O_NONBLOCK); // use non-blocking mode

    output_.resize(4);
}

bool JoystickController::step(double t, double dt) {
    output_ << 0, 0, 0, 0;

    // read() returns -1 when queue is empty
    int bytes = read(joy_fd_, &js_, sizeof(struct js_event));
    if (bytes == -1) {
        // nop
    }

    /* see what to do with the event */
    switch (js_.type & ~JS_EVENT_INIT) {
    case JS_EVENT_AXIS:
        axis_[js_.number] = js_.value;
        break;
    case JS_EVENT_BUTTON:
        button_[js_.number] = js_.value;
        break;
    }

    if (print_js_values_) {
        for (int x = 0; x < num_of_axis_; x++) {
            printf("%d: %6d  ", x, axis_[x] );
        }

        for (int x = 0; x < num_of_buttons_; x++) {
            printf("B%d: %d  ", x, button_[x]);
        }
        printf("  \r");
        fflush(stdout);
    }

    double js_axis_max = +32767;
    double js_axis_min = -32767;
    double output_min = -1;
    double output_max = +1;

    double rudder = -sc::scale<double>(axis_[3], js_axis_min, js_axis_max,
                                      output_min, output_max);
    double elevator = -sc::scale<double>(axis_[1], js_axis_min, js_axis_max,
                                        output_min, output_max);
    double aileron = sc::scale<double>(axis_[0], js_axis_min, js_axis_max,
                                        output_min, output_max);
    double throttle = -sc::scale<double>(axis_[2], js_axis_min, js_axis_max,
                                        output_min, output_max);

    output_ << throttle, elevator, aileron, rudder;

    return true;
}
} // namespace controller
} // namespace scrimmage
