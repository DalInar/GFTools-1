#include "four_index_gf_test.hpp"
#include "mpi_guard.hpp"

static const int MASTER=0;


// Check incompatible mesh broadcast
TEST_F(FourIndexGFTest,MpiWrongBroadcast)
{
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    alps::gf::matsubara_index omega; omega=4;
    alps::gf::momentum_index i; i=2;
    alps::gf::momentum_index j=alps::gf::momentum_index(3);
    alps::gf::index sigma(1);

    const int f=(rank==MASTER)?1:2;
    alps::gf::matsubara_gf gf_wrong(matsubara_mesh(f*beta,f*nfreq),
                                    alps::gf::momentum_index_mesh(5,1),
                                    alps::gf::momentum_index_mesh(5,1),
                                    alps::gf::index_mesh(nspins));
    gf_wrong.initialize();
    if (rank==MASTER) {
      gf_wrong(omega,i,j,sigma)=std::complex<double>(3,4);
    }

    gf_wrong.broadcast(MASTER,MPI_COMM_WORLD);
    
    {
      std::complex<double> x=gf_wrong(omega,i,j,sigma);
        EXPECT_NEAR(3, x.real(),1.e-10);
        EXPECT_NEAR(4, x.imag(),1.e-10);
    }
}

// for testing MPI, we need main()
int main(int argc, char**argv)
{
    MPI_Init(&argc, &argv);
    ::testing::InitGoogleTest(&argc, argv);
    
    Mpi_guard guard(MASTER,"four_index_gf_test_mismatched-mpi.dat.");

    int rc=RUN_ALL_TESTS();

    if (!guard.check_sig_files_ok(get_number_of_bcasts())) {
        MPI_Abort(MPI_COMM_WORLD, 1); // otherwise it may get stuck in MPI_Finalize().
        // downside is the test aborts, rather than reports failure!
    }

    MPI_Finalize();
    return rc;
}
