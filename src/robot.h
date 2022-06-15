class robot
{
    float wheel_size;
    public:
    int requite_heartbeat();
    int mode_auto();
    int mode_manual();
    int set_speed(float v);
    int move_forward(void);
    int move_backward(void);
    int move_continuously(void);
    int stop(void);
    int cont(void);
    int e_stop(void);
    int get_speed(void);
    int get_position(void);
    int get_torque(void);
    int get_status(void);
    int get_water_level(void);
    int start_pump(void);
    int stop_pump(void);
    int get_water_pressure(void);
};