/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 *
 * helloBPBZip2Writer_nompi.cpp sequential non-mpi version of helloBPBZip2Writer
 * using BZip2 http://www.bzip.org/
 *
 *  Created on: Jul 26, 2017
 *      Author: William F Godoy godoywf@ornl.gov
 */

#include <ios>       //std::ios_base::failure
#include <iostream>  //std::cout
#include <numeric>   //std::iota
#include <stdexcept> //std::invalid_argument std::exception
#include <vector>

#include <adios2.h>

int main(int argc, char *argv[])
{
    /** Application variable uints from 0 to 1000 */
    std::vector<unsigned int> myUInts(1000);
    std::iota(myUInts.begin(), myUInts.end(), 0.f);
    const std::size_t Nx = myUInts.size();
    const std::size_t inputBytes = Nx * sizeof(unsigned int);

    try
    {
        /** ADIOS class factory of IO class objects, DebugON is recommended */
        adios2::ADIOS adios(adios2::DebugON);

        // Get a Transform of type BZip2
        adios2::Operator adiosBZip2 =
            adios.DefineOperator("BZip2VariableCompressor", "bzip2");

        /*** IO class object: settings and factory of Settings: Variables,
         * Parameters, Transports, and Execution: Engines */
        adios2::IO bpIO = adios.DeclareIO("BPFile_N2N_BZip2");

        /** global array : name, { shape (total) }, { start (local) }, { count
         * (local) }, all are constant dimensions */
        adios2::Variable<unsigned int> bpUInts =
            bpIO.DefineVariable<unsigned int>("bpUInts", {}, {}, {Nx},
                                              adios2::ConstantDims);

        // 1st way: adding transform metadata to variable to Engine can decide:
        bpUInts.AddOperator(adiosBZip2, {{"BlockSize100K", "10"}});

        // 2nd way: treat Transforms as wrappers to underlying library:
        const std::size_t estimatedSize =
            adiosBZip2.BufferMaxSize(Nx * bpUInts.Sizeof());
        std::vector<char> compressedBuffer(estimatedSize);
        size_t compressedSize = adiosBZip2.Compress(
            myUInts.data(), bpUInts.Count(), bpUInts.Sizeof(), bpUInts.Type(),
            compressedBuffer.data(), {{"BlockSize100K", "9"}});

        compressedBuffer.resize(compressedSize);

        std::cout << "Compression summary:\n";
        std::cout << "Input data size: " << inputBytes << " bytes\n";
        std::cout << "BZip2 estimated output size: " << estimatedSize
                  << " bytes\n";
        std::cout << "BZip2 final output size: " << compressedSize
                  << " bytes\n\n";

        // Allocate original data size
        std::vector<unsigned int> decompressedBuffer(Nx);
        size_t decompressedSize = adiosBZip2.Decompress(
            compressedBuffer.data(), compressedSize, decompressedBuffer.data(),
            decompressedBuffer.size() * sizeof(unsigned int));

        std::cout << "Decompression summary:\n";
        std::cout << "Decompressed size: " << decompressedSize << " bytes\n";
        std::cout << "Data:\n";

        for (const auto number : decompressedBuffer)
        {
            if (number % 25 == 0)
            {
                std::cout << "\n";
            }
            std::cout << number << " ";
        }
        std::cout << "\n";
    }
    catch (std::invalid_argument &e)
    {
        std::cout << "Invalid argument exception, STOPPING PROGRAM\n";
        std::cout << e.what() << "\n";
    }
    catch (std::ios_base::failure &e)
    {
        std::cout << "IO System base failure exception, STOPPING PROGRAM\n";
        std::cout << e.what() << "\n";
    }
    catch (std::exception &e)
    {
        std::cout << "Exception, STOPPING PROGRAM\n";
        std::cout << e.what() << "\n";
    }

    return 0;
}
