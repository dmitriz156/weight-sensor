#include "kalman.h"

void kalman_init(kalman_filter_t *filter, double initial_estimate, double initial_error)
{
  filter->est = initial_estimate;
  filter->err = initial_error;
}

double kalman_filtering(kalman_filter_t *filter, double measurement, double measurement_error, double process_noise)
{
  double prediction      = filter->est;
  double prediction_error = filter->err + process_noise;

  double gain = prediction_error / (prediction_error + measurement_error);
  filter->est = prediction + gain * (measurement - prediction);
  filter->err = (1 - gain) * prediction_error;
  return filter->est;
}
