typedef struct
{
  double est;
  double err;
} kalman_filter_t;

void kalman_init(kalman_filter_t *filter, double initial_estimate, double initial_error);
double kalman_filtering(kalman_filter_t *filter, double measurement, double measurement_error, double process_noise);
