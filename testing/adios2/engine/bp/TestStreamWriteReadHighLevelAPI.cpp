/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 */
#include <cstdint>
#include <cstring>

#include <iostream>
#include <stdexcept>

#include <adios2.h>

#include <gtest/gtest.h>

#include "../SmallTestData.h"

class StreamwriteReadHighLevelAPI : public ::testing::Test
{
public:
    StreamwriteReadHighLevelAPI() = default;

    SmallTestData m_TestData;
};

//******************************************************************************
// 1D 1x8 test data
//******************************************************************************

// ADIOS2 BP write, native ADIOS1 read
TEST_F(StreamwriteReadHighLevelAPI, ADIOS2BPwriteRead1D8)
{
    // Each process would write a 1x8 array and all processes would
    // form a mpiSize * Nx 1D array
    const std::string fname("ADIOS2BPwriteRead1D8_hl.bp");

    int mpiRank = 0, mpiSize = 1;
    // Number of rows
    const size_t Nx = 8;

    // Number of steps
    const size_t NSteps = 3;

#ifdef ADIOS2_HAVE_MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);
#endif

    // write test data using BP
    {
#ifdef ADIOS2_HAVE_MPI
        adios2::fstream oStream(fname, adios2::fstream::out, MPI_COMM_WORLD);
#else
        adios2::fstream oStream(fname, adios2::fstream::out);
#endif
        // write globals
        oStream.write("gi8", m_TestData.I8.front());
        oStream.write("gi16", m_TestData.I16.front());
        oStream.write("gi32", m_TestData.I32.front());
        oStream.write("gi64", m_TestData.I64.front());
        oStream.write("gu8", m_TestData.U8.front());
        oStream.write("gu16", m_TestData.U16.front());
        oStream.write("gu32", m_TestData.U32.front());
        oStream.write("gu64", m_TestData.U64.front());
        oStream.write("gr32", m_TestData.R32.front());
        oStream.write("gr64", m_TestData.R64.front());

        const adios2::Dims shape{static_cast<size_t>(Nx * mpiSize)};
        const adios2::Dims start{static_cast<size_t>(Nx * mpiRank)};
        const adios2::Dims count{Nx};

        for (size_t step = 0; step < NSteps; ++step)
        {
            // Generate test data for each process uniquely
            SmallTestData stepData = generateNewSmallTestData(
                m_TestData, static_cast<int>(step), mpiRank, mpiSize);

            oStream.write("iString", stepData.S1);
            oStream.write("i8", stepData.I8.data(), shape, start, count);
            oStream.write("i16", stepData.I16.data(), shape, start, count);
            oStream.write("i32", stepData.I32.data(), shape, start, count);
            oStream.write("i64", stepData.I64.data(), shape, start, count);
            oStream.write("u8", stepData.U8.data(), shape, start, count);
            oStream.write("u16", stepData.U16.data(), shape, start, count);
            oStream.write("u32", stepData.U32.data(), shape, start, count);
            oStream.write("u64", stepData.U64.data(), shape, start, count);
            oStream.write("r32", stepData.R32.data(), shape, start, count);
            oStream.write("r64", stepData.R64.data(), shape, start, count,
                          adios2::endl);
        }
        oStream.close();
    }

    // READ
    {
#ifdef ADIOS2_HAVE_MPI
        adios2::fstream iStream(fname, adios2::fstream::in, MPI_COMM_WORLD);
#else
        adios2::fstream iStream(fname, adios2::fstream::in);
#endif

        const adios2::Dims start{mpiRank * Nx};
        const adios2::Dims count{Nx};

        int8_t gi8 = 0;
        int16_t gi16 = 0;
        int32_t gi32 = 0;
        int64_t gi64 = 0;
        uint8_t gu8 = 0;
        uint16_t gu16 = 0;
        uint32_t gu32 = 0;
        uint64_t gu64 = 0;
        float gr32 = -1.f;
        double gr64 = -1.f;

        iStream.read("gi8", gi8);
        iStream.read("gi16", gi16);
        iStream.read("gi32", gi32);
        iStream.read("gi64", gi64);
        iStream.read("gu8", gu8);
        iStream.read("gu16", gu16);
        iStream.read("gu32", gu32);
        iStream.read("gu64", gu64);
        iStream.read("gr32", gr32);
        iStream.read("gr64", gr64);

        EXPECT_EQ(gi8, m_TestData.I8.front());
        EXPECT_EQ(gi16, m_TestData.I16.front());
        EXPECT_EQ(gi32, m_TestData.I32.front());
        EXPECT_EQ(gi64, m_TestData.I64.front());
        EXPECT_EQ(gu8, m_TestData.U8.front());
        EXPECT_EQ(gu16, m_TestData.U16.front());
        EXPECT_EQ(gu32, m_TestData.U32.front());
        EXPECT_EQ(gu64, m_TestData.U64.front());
        EXPECT_EQ(gr32, m_TestData.R32.front());
        EXPECT_EQ(gr64, m_TestData.R64.front());

        auto vgi8 = iStream.read<int8_t>("gi8");
        auto vgi16 = iStream.read<int16_t>("gi16");
        auto vgi32 = iStream.read<int32_t>("gi32");
        auto vgi64 = iStream.read<int64_t>("gi64");
        auto vgu8 = iStream.read<uint8_t>("gu8");
        auto vgu16 = iStream.read<uint16_t>("gu16");
        auto vgu32 = iStream.read<uint32_t>("gu32");
        auto vgu64 = iStream.read<uint64_t>("gu64");
        auto vgr32 = iStream.read<float>("gr32");
        auto vgr64 = iStream.read<double>("gr64");

        EXPECT_EQ(vgi8.front(), m_TestData.I8.front());
        EXPECT_EQ(vgi16.front(), m_TestData.I16.front());
        EXPECT_EQ(vgi32.front(), m_TestData.I32.front());
        EXPECT_EQ(vgi64.front(), m_TestData.I64.front());
        EXPECT_EQ(vgu8.front(), m_TestData.U8.front());
        EXPECT_EQ(vgu16.front(), m_TestData.U16.front());
        EXPECT_EQ(vgu32.front(), m_TestData.U32.front());
        EXPECT_EQ(vgu64.front(), m_TestData.U64.front());
        EXPECT_EQ(vgr32.front(), m_TestData.R32.front());
        EXPECT_EQ(vgr64.front(), m_TestData.R64.front());

        size_t t = 0;
        while (!iStream.eof())
        {
            auto IString = iStream.read<std::string>("iString");
            auto I8 = iStream.read<int8_t>("i8", start, count);
            auto I16 = iStream.read<int16_t>("i16", start, count);
            auto I32 = iStream.read<int32_t>("i32", start, count);
            auto I64 = iStream.read<int64_t>("i64", start, count);
            auto U8 = iStream.read<uint8_t>("u8", start, count);
            auto U16 = iStream.read<uint16_t>("u16", start, count);
            auto U32 = iStream.read<uint32_t>("u32", start, count);
            auto U64 = iStream.read<uint64_t>("u64", start, count);
            auto R32 = iStream.read<float>("r32", start, count);
            auto R64 = iStream.read<double>("r64", start, count, adios2::endl);

            SmallTestData currentTestData = generateNewSmallTestData(
                m_TestData, static_cast<int>(t), mpiRank, mpiSize);

            EXPECT_EQ(IString.front(), currentTestData.S1);

            for (size_t i = 0; i < Nx; ++i)
            {
                std::stringstream ss;
                ss << "t=" << t << " i=" << i << " rank=" << mpiRank;
                std::string msg = ss.str();

                EXPECT_EQ(I8[i], currentTestData.I8[i]) << msg;
                EXPECT_EQ(I16[i], currentTestData.I16[i]) << msg;
                EXPECT_EQ(I32[i], currentTestData.I32[i]) << msg;
                EXPECT_EQ(I64[i], currentTestData.I64[i]) << msg;
                EXPECT_EQ(U8[i], currentTestData.U8[i]) << msg;
                EXPECT_EQ(U16[i], currentTestData.U16[i]) << msg;
                EXPECT_EQ(U32[i], currentTestData.U32[i]) << msg;
                EXPECT_EQ(U64[i], currentTestData.U64[i]) << msg;
                EXPECT_EQ(R32[i], currentTestData.R32[i]) << msg;
                EXPECT_EQ(R64[i], currentTestData.R64[i]) << msg;
            }

            ++t;
        }

        iStream.close();
    }
}

//******************************************************************************
// 2D 2x4 test data
//******************************************************************************

// ADIOS2 BP write, native ADIOS1 read
TEST_F(StreamwriteReadHighLevelAPI, ADIOS2BPwriteRead2D2x4)
{
    // Each process would write a 2x4 array and all processes would
    // form a 2D 2 * (numberOfProcess*Nx) matrix where Nx is 4 here
    const std::string fname("ADIOS2BPwriteRead2D2x4Test_hl.bp");

    int mpiRank = 0, mpiSize = 1;
    // Number of rows
    const std::size_t Nx = 4;

    // Number of rows
    const std::size_t Ny = 2;

    // Number of steps
    const std::size_t NSteps = 3;

#ifdef ADIOS2_HAVE_MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);
#endif

    // write test data using ADIOS2
    {
#ifdef ADIOS2_HAVE_MPI
        adios2::fstream oStream(fname, adios2::fstream::out, MPI_COMM_WORLD);
#else
        adios2::fstream oStream(fname, adios2::fstream::out);
#endif

        const adios2::Dims shape{Ny, static_cast<size_t>(Nx * mpiSize)};
        const adios2::Dims start{0, static_cast<size_t>(mpiRank * Nx)};
        const adios2::Dims count{Ny, Nx};

        for (size_t step = 0; step < NSteps; ++step)
        {
            // Generate test data for each process uniquely
            SmallTestData stepData = generateNewSmallTestData(
                m_TestData, static_cast<int>(step), mpiRank, mpiSize);

            oStream.write("iString", stepData.S1);
            oStream.write("i8", stepData.I8.data(), shape, start, count);
            oStream.write("i16", stepData.I16.data(), shape, start, count);
            oStream.write("i32", stepData.I32.data(), shape, start, count);
            oStream.write("i64", stepData.I64.data(), shape, start, count);
            oStream.write("u8", stepData.U8.data(), shape, start, count);
            oStream.write("u16", stepData.U16.data(), shape, start, count);
            oStream.write("u32", stepData.U32.data(), shape, start, count);
            oStream.write("u64", stepData.U64.data(), shape, start, count);
            oStream.write("r32", stepData.R32.data(), shape, start, count);
            oStream.write("r64", stepData.R64.data(), shape, start, count,
                          adios2::endl);
        }

        // Close the file
        oStream.close();
    }

    // READ
    {
#ifdef ADIOS2_HAVE_MPI
        adios2::fstream iStream(fname, adios2::fstream::in, MPI_COMM_WORLD);
#else
        adios2::fstream iStream(fname, adios2::fstream::in);
#endif
        const adios2::Dims start{0, static_cast<size_t>(mpiRank * Nx)};
        const adios2::Dims count{Ny, Nx};
        const adios2::Box<adios2::Dims> sel(start, count);

        // for (size_t t = 0; t < NSteps; ++t)
        size_t t = 0;
        while (!iStream.eof())
        {
            auto IString = iStream.read<std::string>("iString");
            auto I8 = iStream.read<int8_t>("i8", start, count);
            auto I16 = iStream.read<int16_t>("i16", start, count);
            auto I32 = iStream.read<int32_t>("i32", start, count);
            auto I64 = iStream.read<int64_t>("i64", start, count);
            auto U8 = iStream.read<uint8_t>("u8", start, count);
            auto U16 = iStream.read<uint16_t>("u16", start, count);
            auto U32 = iStream.read<uint32_t>("u32", start, count);
            auto U64 = iStream.read<uint64_t>("u64", start, count);
            auto R32 = iStream.read<float>("r32", start, count);
            auto R64 = iStream.read<double>("r64", start, count, adios2::endl);

            // Generate test data for each rank uniquely
            SmallTestData currentTestData = generateNewSmallTestData(
                m_TestData, static_cast<int>(t), mpiRank, mpiSize);

            EXPECT_EQ(IString.front(), currentTestData.S1);

            for (size_t i = 0; i < Nx * Ny; ++i)
            {
                std::stringstream ss;
                ss << "t=" << t << " i=" << i << " rank=" << mpiRank;
                std::string msg = ss.str();

                EXPECT_EQ(I8[i], currentTestData.I8[i]) << msg;
                EXPECT_EQ(I16[i], currentTestData.I16[i]) << msg;
                EXPECT_EQ(I32[i], currentTestData.I32[i]) << msg;
                EXPECT_EQ(I64[i], currentTestData.I64[i]) << msg;
                EXPECT_EQ(U8[i], currentTestData.U8[i]) << msg;
                EXPECT_EQ(U16[i], currentTestData.U16[i]) << msg;
                EXPECT_EQ(U32[i], currentTestData.U32[i]) << msg;
                EXPECT_EQ(U64[i], currentTestData.U64[i]) << msg;
                EXPECT_EQ(R32[i], currentTestData.R32[i]) << msg;
                EXPECT_EQ(R64[i], currentTestData.R64[i]) << msg;
            }
            ++t;
        }
        iStream.close();
    }
}

//******************************************************************************
// 2D 4x2 test data
//******************************************************************************

TEST_F(StreamwriteReadHighLevelAPI, ADIOS2BPwriteRead2D4x2)
{
    // Each process would write a 4x2 array and all processes would
    // form a 2D 4 * (NumberOfProcess * Nx) matrix where Nx is 2 here
    const std::string fname("ADIOS2BPwriteRead2D4x2Test_hl.bp");

    int mpiRank = 0, mpiSize = 1;
    // Number of rows
    const std::size_t Nx = 2;
    // Number of cols
    const std::size_t Ny = 4;

    // Number of steps
    const std::size_t NSteps = 3;

#ifdef ADIOS2_HAVE_MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);
#endif

    // write test data using ADIOS2
    {
#ifdef ADIOS2_HAVE_MPI
        adios2::fstream oStream(fname, adios2::fstream::out, MPI_COMM_WORLD);
#else
        adios2::fstream oStream(fname, adios2::fstream::out);
#endif

        // Declare 2D variables (4 * (NumberOfProcess * Nx))
        // The local process' part (start, count) can be defined now or later
        // before write().
        adios2::Dims shape{Ny, static_cast<std::size_t>(mpiSize * Nx)};
        adios2::Dims start{0, static_cast<std::size_t>(mpiRank * Nx)};
        adios2::Dims count{Ny, Nx};

        for (size_t step = 0; step < NSteps; ++step)
        {
            // Generate test data for each process uniquely
            SmallTestData stepData = generateNewSmallTestData(
                m_TestData, static_cast<int>(step), mpiRank, mpiSize);

            oStream.write("iString", stepData.S1);
            oStream.write("i8", stepData.I8.data(), shape, start, count);
            oStream.write("i16", stepData.I16.data(), shape, start, count);
            oStream.write("i32", stepData.I32.data(), shape, start, count);
            oStream.write("i64", stepData.I64.data(), shape, start, count);
            oStream.write("u8", stepData.U8.data(), shape, start, count);
            oStream.write("u16", stepData.U16.data(), shape, start, count);
            oStream.write("u32", stepData.U32.data(), shape, start, count);
            oStream.write("u64", stepData.U64.data(), shape, start, count);
            oStream.write("r32", stepData.R32.data(), shape, start, count);
            oStream.write("r64", stepData.R64.data(), shape, start, count,
                          adios2::endl);
        }

        EXPECT_THROW(oStream.write<int16_t>("i8", 1), std::invalid_argument);
        oStream.close();
    }

    {
#ifdef ADIOS2_HAVE_MPI
        adios2::fstream iStream(fname, adios2::fstream::in, MPI_COMM_WORLD);
#else
        adios2::fstream iStream(fname, adios2::fstream::in);
#endif

        const adios2::Dims start{0, static_cast<size_t>(mpiRank * Nx)};
        const adios2::Dims count{Ny, Nx};
        const adios2::Box<adios2::Dims> sel(start, count);

        // for (size_t t = 0; t < NSteps; ++t)
        size_t t = 0;
        while (!iStream)
        {
            auto IString = iStream.read<std::string>("iString");
            auto I8 = iStream.read<int8_t>("i8", start, count);
            auto I16 = iStream.read<int16_t>("i16", start, count);
            auto I32 = iStream.read<int32_t>("i32", start, count);
            auto I64 = iStream.read<int64_t>("i64", start, count);
            auto U8 = iStream.read<uint8_t>("u8", start, count);
            auto U16 = iStream.read<uint16_t>("u16", start, count);
            auto U32 = iStream.read<uint32_t>("u32", start, count);
            auto U64 = iStream.read<uint64_t>("u64", start, count);
            auto R32 = iStream.read<float>("r32", start, count);
            auto R64 = iStream.read<double>("r64", start, count, adios2::endl);

            // Generate test data for each rank uniquely
            SmallTestData currentTestData = generateNewSmallTestData(
                m_TestData, static_cast<int>(t), mpiRank, mpiSize);

            EXPECT_EQ(IString.front(), currentTestData.S1);

            for (size_t i = 0; i < Nx * Ny; ++i)
            {
                std::stringstream ss;
                ss << "t=" << t << " i=" << i << " rank=" << mpiRank;
                std::string msg = ss.str();

                EXPECT_EQ(I8[i], currentTestData.I8[i]) << msg;
                EXPECT_EQ(I16[i], currentTestData.I16[i]) << msg;
                EXPECT_EQ(I32[i], currentTestData.I32[i]) << msg;
                EXPECT_EQ(I64[i], currentTestData.I64[i]) << msg;
                EXPECT_EQ(U8[i], currentTestData.U8[i]) << msg;
                EXPECT_EQ(U16[i], currentTestData.U16[i]) << msg;
                EXPECT_EQ(U32[i], currentTestData.U32[i]) << msg;
                EXPECT_EQ(U64[i], currentTestData.U64[i]) << msg;
                EXPECT_EQ(R32[i], currentTestData.R32[i]) << msg;
                EXPECT_EQ(R64[i], currentTestData.R64[i]) << msg;
            }
            ++t;
        }
        iStream.close();
    }
}

//******************************************************************************
// main
//******************************************************************************

int main(int argc, char **argv)
{
#ifdef ADIOS2_HAVE_MPI
    MPI_Init(nullptr, nullptr);
#endif

    int result;
    ::testing::InitGoogleTest(&argc, argv);
    result = RUN_ALL_TESTS();

#ifdef ADIOS2_HAVE_MPI
    MPI_Finalize();
#endif

    return result;
}
