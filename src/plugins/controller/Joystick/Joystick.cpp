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

#include <scrimmage/plugins/controller/Joystick/Joystick.h>

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
                scrimmage::controller::Joystick,
                Joystick_plugin)

namespace scrimmage {
namespace controller {

// Define your user buttons
enum Button {
    ButtonMenu,
    ButtonConfirm,
    MouseX,
    MouseY,
    LeftMouseButton
};

Joystick::Joystick() {
}

void Joystick::init(std::map<std::string, std::string> &params) {

    print_js_values_ = sc::get<bool>("print_raw_joystick_values", params, false);

    if( ( joy_fd_ = open( "/dev/input/js0" , O_RDONLY)) == -1 ) {
		cout << "couln't open joystick" << endl;
	}

    ioctl( joy_fd_, JSIOCGAXES, &num_of_axis_ );
	ioctl( joy_fd_, JSIOCGBUTTONS, &num_of_buttons_ );
	ioctl( joy_fd_, JSIOCGNAME(80), &name_of_joystick_ );

	axis_ = (int *) calloc( num_of_axis_, sizeof( int ) );
	button_ = (char *) calloc( num_of_buttons_, sizeof( char ) );

    cout << "Joystick detected:" << name_of_joystick_ << endl;
    cout << "\t " << num_of_axis_ << " axis" << endl;
    cout << "\t " << num_of_buttons_ << " buttons" << endl;

	fcntl( joy_fd_, F_SETFL, O_NONBLOCK );	/* use non-blocking mode */

    output_.resize(4);
}

bool Joystick::step(double t, double dt) {
    output_ << 0, 0, 0, 0;

    ssize_t bytes = read(joy_fd_, &js_, sizeof(struct js_event));
    //if (bytes != sizeof(js_)) {
    //    cout << "Error during joystick read." << endl;
    //}

    /* see what to do with the event */
    switch (js_.type & ~JS_EVENT_INIT) {
    case JS_EVENT_AXIS:
        axis_[ js_.number ] = js_.value;
        break;
    case JS_EVENT_BUTTON:
        button_ [ js_.number ] = js_.value;
        break;
    }

    for (int x = 0; x < num_of_axis_; x++) {
        printf("%d: %6d  ", x, axis_[x] );
    }

    for(int x = 0 ; x < num_of_buttons_ ; ++x ) {
        printf("B%d: %d  ", x, button_[x] );
    }

    printf("  \r");
    fflush(stdout);

    return true;
}
} // namespace controller
} // namespace scrimmage
