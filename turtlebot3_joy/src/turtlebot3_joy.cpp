#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>

#define KEY_A     0
#define KEY_B     1
#define KEY_X     2
#define KEY_Y     3
#define KEY_LT      5
#define KEY_RT      4
#define KEY_LB      6
#define KEY_RB      7

#define AXES_HORIENTAL 0
#define AXES_VERTICAL  1

float linear_a = 0.1;
float angular_a = 0.2;


// create the TeleopTurtle class and define the joyCallback function that will take a joy msg
class TeleopTurtle
{
public:
  TeleopTurtle();

private:
  void joyCallback(const sensor_msgs::Joy::ConstPtr& Joy);

  ros::NodeHandle nh_;

  int linear_, angular_;   // used to define which axes of the joystick will control our turtle
  double l_scale_, a_scale_;
  ros::Publisher vel_pub_;
  ros::Subscriber joy_sub_;

};


TeleopTurtle::TeleopTurtle(): linear_(1), angular_(2)
{
  //  initialize some parameters
  nh_.param("axis_linear", linear_, linear_);  
  nh_.param("axis_angular", angular_, angular_);
  nh_.param("scale_angular", a_scale_, a_scale_);
  nh_.param("scale_linear", l_scale_, l_scale_);

  // create a publisher that will advertise on the command_velocity topic of the turtle
  vel_pub_ = nh_.advertise<geometry_msgs::Twist>("cmd_vel", 1);

  // subscribe to the joystick topic for the input to drive the turtle
  joy_sub_ = nh_.subscribe<sensor_msgs::Joy>("joy", 10, &TeleopTurtle::joyCallback, this);
}


void TeleopTurtle::joyCallback(const sensor_msgs::Joy::ConstPtr& Joy)
{
  geometry_msgs::Twist twist;

  // change by matthew on 2021/1/15
  // take the data from the joystick and manipulate it by scaling it and using independent axes to control the linear and angular velocities of the turtle
//  twist.angular.z = a_scale_*joy->axes[angular_];
//  twist.linear.x = l_scale_*joy->axes[linear_];
//    linear_max = l_scale_;
//    angular_max = a_scale_;
    static bool flag=1;
    if (Joy->axes[KEY_LT]!=0&&Joy->axes[KEY_RT]!=0)
        flag=0;
    twist.linear.x = (-Joy->axes[KEY_LT] + 1) * l_scale_;
    if(twist.linear.x==0)
        twist.linear.x = Joy->axes[AXES_VERTICAL] * l_scale_;
    twist.angular.z = Joy->axes[0] * a_scale_;
    if (Joy->buttons[KEY_RB])
    {
        if (Joy->buttons[KEY_Y])
            l_scale_ += linear_a;
        if (Joy->buttons[KEY_B])
            a_scale_ -= angular_a;
        if (Joy->buttons[KEY_A])
            l_scale_ -= linear_a;
        if (Joy->buttons[KEY_X])
            a_scale_ += angular_a;
    }
    if (flag||Joy->buttons[KEY_LB])
    {
        twist.linear.x = 0;
        twist.angular.z = 0;
    }
//    std::cout << "Angular is " << twist.angular.z << std::endl;
  vel_pub_.publish(twist); 
}


int main(int argc, char** argv)
{
  // initialize our ROS node, create a teleop_turtle, and spin our node until Ctrl-C is pressed
  ros::init(argc, argv, "teleop_joy_turtle");
  TeleopTurtle teleop_turtle;
  ros::spin();
}
