

void compass(void)
{

  int ret;
  int ard = 0; /* set to arduino number in ini file */
  int calibrate = 0;  /* set to 1 in order to create a new x/y calibration table (see above) */

  float compassdegrees;
  
  int *avionics_on = link_dataref_int("sim/cockpit/electrical/avionics_on");

  //  float *heading_mag = link_dataref_flt("sim/flightmodel/position/magpsi",-1);
  float *heading_mag = link_dataref_flt("sim/cockpit/autopilot/heading_mag",0);      
  
  /* read encoder at inputs 13 and 15 */
  if (*heading_mag == FLT_MISS) *heading_mag = 0.0;
  ret = encoder_inputf(ard, 8, 9, heading_mag, 5.0, 1);
  if (ret == 1) {
    if (*heading_mag > 360.) *heading_mag -= 360.0;
    if (*heading_mag < 0.0) *heading_mag += 360.0;
    printf("Heading (degrees): %f \n",*heading_mag);
  }

  if (calibrate) {
    compassdegrees = *heading_mag;
  } else {
    compassdegrees = interpolate(*heading_mag);
  }
  int y = (int) 255.0 * cos(compassdegrees*3.14/180.0);
  int x = (int) 255.0 * sin(compassdegrees*3.14/180.0);
  int absx = abs(x);
  int absy = abs(y);
  int revabsx = 255-abs(x);
  int revabsy = 255-abs(y);
  int zero = 0;
  int one = 1;

  if (x > 0) {
    ret = analog_output(ard,5,&absx);
    ret = digital_output(ard,3,&zero);
  } else {
    ret = analog_output(ard,5,&revabsx);
    ret = digital_output(ard,3,&one);
  }
  
  if (y < 0) {
    ret = analog_output(ard,6,&absy);
    ret = digital_output(ard,7,&zero);
  } else {
    ret = analog_output(ard,6,&revabsy);
    ret = digital_output(ard,7,&one);
  }
  
  /* turn on Compass light (LED) connected to second output (#1) if avionics are on */
  if (*avionics_on == INT_MISS) *avionics_on = 1;
  ret = digital_output(ard, 2, avionics_on);
  
}
