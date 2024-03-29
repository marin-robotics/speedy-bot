#include "main.h"

pros::Controller master(pros::E_CONTROLLER_MASTER);
pros::Motor left_mtr_0(18);
pros::Motor left_mtr_1(19);
pros::Motor left_mtr_2(20);
pros::Motor right_mtr_0(8, true);
pros::Motor right_mtr_1(9, true);
pros::Motor right_mtr_2(10, true);

pros::Motor lift_mtr(7, true);
pros::Motor claw_mtr(17, true);

void move_left(int speed) {
	left_mtr_0 = speed;
	left_mtr_1 = speed;
	left_mtr_2 = speed;
}

void move_right(int speed) {
	right_mtr_0 = speed;
	right_mtr_1 = speed;
	right_mtr_2 = speed;
}

int controller_reversed = 1;
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	pros::lcd::initialize();

	pros::lcd::register_btn1_cb(on_center_button);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
	// forward
	move_right(50);
	move_left(50);
	pros::delay(1000);
	// back
	move_right(-50);
	move_left(-50);
	pros::delay(1000);
	// turn left
	move_right(50);
	move_left(-50);
	pros::delay(700);
	// stop movement
	move_right(0);
	move_left(0);
	// deploy claw
	lift_mtr = -127;
	pros::delay(500);
	lift_mtr = 0;
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
	bool holding_cube = false;
	while (true) {
		if (master.get_digital(DIGITAL_A))
		{
			controller_reversed *= -1;
		}

		if (controller_reversed == -1)
		{
			move_left(master.get_analog(ANALOG_RIGHT_Y) * controller_reversed);
			move_right(master.get_analog(ANALOG_LEFT_Y) * controller_reversed);
		}
		else
		{
			move_left(master.get_analog(ANALOG_LEFT_Y));
			move_right(master.get_analog(ANALOG_RIGHT_Y));
		}

		if (master.get_digital(DIGITAL_L1)) {
			lift_mtr = 127;
		} else if (master.get_digital(DIGITAL_L2)) {
			lift_mtr = -127;
		} else {
			lift_mtr = 0;
		}

		if (master.get_digital(DIGITAL_R1)) {
			claw_mtr = 50;
			holding_cube = true;
		} else if (master.get_digital(DIGITAL_R2)) {
			claw_mtr = -50;
			holding_cube = false;
		} else if (holding_cube) {
			claw_mtr = 20;
		} else {
			claw_mtr = 0;
		}

		pros::delay(20);
	}
}
