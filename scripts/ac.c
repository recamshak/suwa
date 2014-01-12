#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <argp.h>


#define PULSE_BIT       0x01000000

typedef enum {
  false,
  true
} bool;

const unsigned int HEADER[] = {
  3740 | PULSE_BIT,
  1940
};

const unsigned int FOOTER = 450 | PULSE_BIT;

const unsigned int ONE[] = {
  440 | PULSE_BIT,
  1430
};

const unsigned int ZERO[] = {
  440 | PULSE_BIT,
  490
};

typedef enum {
  stop   = 0x00,
  cooler = 0x04,
  heater = 0x08
} ac_mode;

typedef enum {
  never,
  off_in_30_min,
  off_in_1_hour,
  off_in_2_hours,
  off_in_3_hours,
  off_in_5_hours
} off_timer;

typedef struct {
  ac_mode     mode;
  int         temperature;
  bool        ion;
  off_timer   off_timer;
} command;

typedef struct ir_data {
  unsigned char data[13];
} ir_data;

typedef struct ir_signal {
  unsigned int signal[sizeof(ir_data) * 8 // each bit of data
                      * 2                 // needs two signal int.
                      + 3];               // 2 for header and 1 for footer
} ir_signal;

const ir_data ir_data_default = {
  0x55, 0x5A, 0xF3, 0x08, // ID
  0x00, 0x88, 0x80, 0x00,
  0x10, 0x01, 0x00, 0x07,
  0x80
};


void get_data(const command *cmd, ir_data *data) {
  memcpy(data, &ir_data_default, sizeof(ir_data));


  data_update_checksum(data);
}


void data_update_checksum(ir_data* data) {
  int i;
  char checksum = 0;

  for (i = 0; i < sizeof(ir_data) - 1; i++) {
    checksum ^= data->data[i] & 0x0F;
    checksum ^= (data->data[i] >> 4) & 0x0F;
  }

  checksum ^= (data->data[i] >> 4) & 0x0F;
  data->data[i] |= checksum;
}


void get_signal(const ir_data *data, ir_signal *signal) {
  int i, byte, bit, signal_idx;

  signal->signal[0] = HEADER[0];
  signal->signal[1] = HEADER[1];

  signal_idx = 2;

  for (i = 0; i < sizeof(ir_data); i++) {
    byte = data->data[i];

    for (bit = 0; bit < 8; bit++) {
      if ((byte << bit) & 0x80) {
        signal->signal[signal_idx++] = ONE[0];
        signal->signal[signal_idx++] = ONE[1];
      } else {
        signal->signal[signal_idx++] = ZERO[0];
        signal->signal[signal_idx++] = ZERO[1];
      }
    }
  }

  signal->signal[signal_idx] = FOOTER;
}


void send(const command *cmd, int fd) {
  ir_data data;
  ir_signal signal;

  get_data(cmd, &data);
  get_signal(&data, &signal);

  write(fd, &signal, sizeof(signal));
}




/*

bits 0-3:
  Temperature or self cleaner

  0000: Self cleaner
  0001: 18°C
  ...
  1111: 32°C

bits 20-22:
  air flow power

  010: automatique
  110: 1
  101: 2
  111: 3
  011: 4

bits 32-34:
  air flow direction

  000: automatique
  111: swing

  heater, dryer:
  001: up
  ...
  100: down

  clim, deshumidificateur, ion:
  001: shower
  010: up
  ...
  101: down

bit 44:
  moderate

bit 5 8:
  ion

bits 16-17:
  function

  00: dry/ion
  10: heater
  01: clim
  11: deshumidificateur


bits 68-71:
  checksum

  xor of all the 4bits words including the ID
*/


// void start(int fd, int fct, int temperature) {
//   char ir_code[] = {
//     0x55, 0x5A, 0xF3, 0x08, // ID
//     0x00, 0x88, 0x80, 0x00,
//     0x10, 0x01, 0x00, 0x07,
//     0x80
//   };

//   ir_code[6] |= fct;

//   // get the temperature code
//   temperature -= 17;
//   int temperature_code = 0;

//   // reverse the bit order
//   int i;
//   for (i = 0; i < 4; i++) {
//     temperature_code <<= 1;
//     temperature_code |= (0x01 & temperature);
//     temperature >>= 1;
//   }

//   ir_code[4] |= temperature_code << 4;

//   char checksum = 0;

//   for (i = 0; i < sizeof(ir_code); i++) {
//     checksum ^= ir_code[i] & 0x0F;
//     checksum ^= (ir_code[i] >> 4) & 0x0F;
//   }
//   ir_code[sizeof(ir_code) - 1] |= checksum;

//   send(fd, ir_code, sizeof(ir_code));

//   for (i = 0; i < sizeof(ir_code); i++) {
//     printf("%02X ", ir_code[i]);
//   }
// }
/* The options we understand. */

struct arguments {
  command cmd;
  char *lirc_device;
};

/* Program documentation. */
static char doc[] =
 "Argp example #3 -- a program with options and arguments using argp";

/* A description of the arguments we accept. */
static char args_doc[] = "LIRC_DEVICE COMMAND";

static struct argp_option options[] = {
   {"temperature", 't', "TEMP", OPTION_ARG_OPTIONAL,
    "Set the temperature to TEMP [18-32]°C, default to 20"},

   {"ion", 'i', 0, 0,
    "Activate the Ion Plasmacluster"},

   { 0 }
};



static error_t parse_opt (int key, char *arg, struct argp_state *state) {
  /* Get the input argument from argp_parse, which we
    know is a pointer to our arguments structure. */
  struct arguments *arguments = state->input;

  switch (key)
   {
   case 't':
     arguments->cmd.temperature = atoi(arg);
     break;

   case ARGP_KEY_ARG:
    if (state->arg_num >= 2) {
      /* Too many arguments. */
      argp_usage (state);
    }

    if (state->arg_num == 0) {
      arguments->lirc_device = arg;
    } else {
      if (strcmp(arg, "stop") == 0) {
        arguments->cmd.mode = stop;
      } else if (strcmp(arg, "cooler") == 0) {
        arguments->cmd.mode = cooler;
      } else if (strcmp(arg, "heater") == 0) {
        arguments->cmd.mode = heater;
      } else {
        /* unknown command */
        argp_usage (state);
      }
    }

    break;

   case ARGP_KEY_END:
     if (state->arg_num < 2)
       /* Not enough arguments. */
       argp_usage (state);
     break;

   default:
     return ARGP_ERR_UNKNOWN;
   }
  return 0;
}


int main(int argc, char** argv) {
  struct arguments arguments;

  struct argp argp = { options, parse_opt, args_doc, doc };
  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  int fd = open(arguments.lirc_device, O_WRONLY);
  send(&arguments.cmd, fd);
	close(fd);

	return 0;
}
