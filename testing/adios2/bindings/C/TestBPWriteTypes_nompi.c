/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 *
 * TestBPWriteTypes.c
 *
 *  Created on: Aug 9, 2017
 *      Author: William F Godoy godoywf@ornl.gov
 */

#include <adios2_c.h>
#include <stddef.h>

#include "SmallTestData_c.h"

int main(int argc, char *argv[])
{
    adios2_adios *adiosH = adios2_init_nompi(adios2_debug_mode_on);

    // IO
    adios2_io *ioH = adios2_declare_io(adiosH, "CArrayTypes");
    // Set engine parameters
    adios2_set_engine(ioH, "BPFileWriter");
    adios2_set_parameter(ioH, "ProfileUnits", "Microseconds");
    adios2_set_parameter(ioH, "Threads", "1");

    // Set transport and parameters
    const unsigned int transportID = adios2_add_transport(ioH, "File");
    adios2_set_transport_parameter(ioH, transportID, "library", "fstream");

    // count dims are allocated in stack
    size_t count[1];
    count[0] = data_Nx;

    // Define variables in ioH
    {
        adios2_define_variable(ioH, "varI8", adios2_type_char, 1, NULL, NULL,
                               count, adios2_constant_dims_true, NULL);
        adios2_define_variable(ioH, "varI16", adios2_type_short, 1, NULL, NULL,
                               count, adios2_constant_dims_true, NULL);
        adios2_define_variable(ioH, "varI32", adios2_type_int, 1, NULL, NULL,
                               count, adios2_constant_dims_true, NULL);
        adios2_define_variable(ioH, "varI64", adios2_type_long_int, 1, NULL,
                               NULL, count, adios2_constant_dims_true, NULL);

        adios2_define_variable(ioH, "varU8", adios2_type_unsigned_char, 1, NULL,
                               NULL, count, adios2_constant_dims_true, NULL);
        adios2_define_variable(ioH, "varU16", adios2_type_unsigned_short, 1,
                               NULL, NULL, count, adios2_constant_dims_true,
                               NULL);
        adios2_define_variable(ioH, "varU32", adios2_type_unsigned_int, 1, NULL,
                               NULL, count, adios2_constant_dims_true, NULL);
        adios2_define_variable(ioH, "varU64", adios2_type_unsigned_long_int, 1,
                               NULL, NULL, count, adios2_constant_dims_true,
                               NULL);

        adios2_define_variable(ioH, "varR32", adios2_type_float, 1, NULL, NULL,
                               count, adios2_constant_dims_true, NULL);
        adios2_define_variable(ioH, "varR64", adios2_type_double, 1, NULL, NULL,
                               count, adios2_constant_dims_true, NULL);
    }
    // get variables
    adios2_variable *varI8 = adios2_inquire_variable(ioH, "varI8");
    adios2_variable *varI16 = adios2_inquire_variable(ioH, "varI16");
    adios2_variable *varI32 = adios2_inquire_variable(ioH, "varI32");
    adios2_variable *varI64 = adios2_inquire_variable(ioH, "varI64");
    adios2_variable *varU8 = adios2_inquire_variable(ioH, "varU8");
    adios2_variable *varU16 = adios2_inquire_variable(ioH, "varU16");
    adios2_variable *varU32 = adios2_inquire_variable(ioH, "varU32");
    adios2_variable *varU64 = adios2_inquire_variable(ioH, "varU64");
    adios2_variable *varR32 = adios2_inquire_variable(ioH, "varR32");
    adios2_variable *varR64 = adios2_inquire_variable(ioH, "varR64");

    // Open Engine handler, Write and Close
    adios2_engine *engineH = adios2_open(ioH, "ctypes.bp", adios2_mode_write);

    adios2_put(engineH, varI8, data_I8, adios2_mode_sync);
    adios2_put(engineH, varI16, data_I16, adios2_mode_sync);
    adios2_put(engineH, varI32, data_I32, adios2_mode_sync);
    adios2_put(engineH, varI64, data_I64, adios2_mode_sync);

    adios2_put(engineH, varU8, data_U8, adios2_mode_sync);
    adios2_put(engineH, varU16, data_U16, adios2_mode_sync);
    adios2_put(engineH, varU32, data_U32, adios2_mode_sync);
    adios2_put(engineH, varU64, data_U64, adios2_mode_sync);

    adios2_put(engineH, varR32, data_R32, adios2_mode_sync);
    adios2_put(engineH, varR64, data_R64, adios2_mode_sync);

    adios2_close(engineH);

    // deallocate adiosH
    adios2_finalize(adiosH);
    return 0;
}
