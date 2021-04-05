#ifndef ENCODER__H__
#define ENCODER__H__

// Initialize the encoder module.
void encoder_init();

// Read the encoder angle in ticks.
int encoder_counts();

// Read the encoder angle in units of 0.1 degrees.
int encoder_angle();

// Reset the encoder position.
int encoder_reset();

#endif