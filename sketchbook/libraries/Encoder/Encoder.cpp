
#include "Encoder.h"

// Yes, all the code is in the header file, to provide the user
// configure options with #define (before they include it), and
// to facilitate some crafty optimizations!

template <typename object_t>
Encoder_internal_state_t<object_t> * Encoder<object_t>::interruptArgs[];


