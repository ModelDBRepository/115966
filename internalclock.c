//
// Simulation program
//

#include<stdio.h>
#include<stdlib.h>
#include<math.h>

// Parameters (p.1034)
#define N 1000			// # of neurons
#define T 1000			// # of time steps
#define Pr 0.1			// connection probability
#define tau 100.0		// temporal integration constant
#define kappa 5.0		// strength of recurrent inhibition
#define I 1.0			// strength of input signals

#define wid(i,j) ((j)+N*(i))
#define zid(t,i) ((i)+N*(t))

extern void init_genrand(unsigned long);
extern double genrand_real2(void);

// This function takes a seed for the random number generator and
// returns the random matrix w_{ij} (Eq.(2.1) in p.1033).
// The matrix is the size of N*N, and the i th row represents the list
// of indices for presynaptic neurons of neuron i.  The connection
// probability is Pr.
// Each row terminates with -1 so that the program can know
// the end of the list.
int *random_matrix_index(const unsigned long seed)
{
  int i, j, n;
  int *w;

  init_genrand(seed);

  w = (int *)malloc(N*N*sizeof(int));
  for(i = 0; i < N; i++){
    n = 0;
    for(j = 0; j < N; j++){
      if (genrand_real2() < Pr){
	w[wid(i,n)] = j;
	n++;
      }
    }
    w[wid(i,n)] = -1;
  }
  return w;
}

// This function returns the T*N vector of neural activity z(t,i) (p. 1033).
// The vector is accessed as a T*N matrix through the function zid(t,i).
double *activity_pattern(void)
{
  int t, i;
  double *z;

  z = (double *)malloc(T*N*sizeof(double));
  for(t = 0; t < T; t++){
    for(i = 0; i < N; i++){
      z[zid(t,i)] = 0;
    }
  }
  return z;
}

// This function takes the neural activity z(t,i) and the value of the
// inter-stimulus interval (ISI) for eyeblink conditioning, and
// generate 3 files: activity.dat, raster.dat, and readout.dat.
void output(const double *z, const int isi)
{
  FILE *file;
  int t, i;
  char buf[1024];
  double r, w[N];

  // "activity.dat" contains neural activity z(t,i), which is used
  // to calculate the similarity index (Eq. (2.2))
  file = fopen("activity.dat", "w");
  for(t = 0; t < T; t++){
    for(i = 0; i < N; i++){
      fprintf(file, "%f\n", z[zid(t,i)]);
    }
  }
  fclose(file);

  // "raster.dat" represents the indices of active neurons (z(t,i)>0).
  // This is a list of pairs of firing time t and neuron index i.
  sprintf(buf, "raster.dat");
  file = fopen(buf, "w");
  for(t = 0; t < T; t++){
    for(i = 0; i < N; i++){
      if (z[zid(t,i)] > 0){
	fprintf(file, "%d %d\n", t, i);
      }
    }
  }
  fclose(file);

  // "readout.dat" represents Net input(t) in p. 1048.
  sprintf(buf, "readout.dat");
  file = fopen(buf, "w");
  // Synaptic weight for neuron i is set at 0 if the neuron is active
  // at the specified ISI; otherwise the weight is 1.
  for(i = 0; i < N; i++){
    if (z[zid(isi,i)] > 0){
      w[i] = 0;
    }else{
      w[i] = 1;
    }
  }
  // Plot the net input
  for(t = 0; t < T; t++){
    r = 0;
    for(i = 0; i < N; i++){
      r += w[i]*z[zid(t,i)];
    }
    fprintf(file, "%d %f\n", t, r);
  }
  fclose(file);
}

// This function takes the random matrix w and the empty array of
// the neural activity z, and fill the array z.
void run(const int *w, double *z)
{
  int t, i, n;
  double u[N], q[N];
  double r;

  const double decay = exp(-1.0/tau);
  const double coef = 2.0*kappa/N;

  for(i = 0; i < N; i++){
    q[i] = 0;
  }

  // Iterative calculation of Eq. (2.1)
  for(t = 1; t < T; t++){
    for(i = 0; i < N; i++){
      q[i] = z[zid(t-1,i)] + decay*q[i];
    }
    for(i = 0; i < N; i++){
      r = 0;
      // the list of presynaptic neurons is terminated with -1.
      for(n = 0; w[wid(i,n)] >= 0; n++){
	r += coef*q[w[wid(i,n)]];
      }	
      u[i] = I - r;
    }
    for(i = 0; i < N; i++){
      z[zid(t,i)] = (u[i] > 0) ? u[i] : 0;
    }
  }
}

int main(int argc, char *argv[])
{
  double *z;
  int *w, isi;

  if (argc < 3){
    fprintf(stderr, "usage: %s <seed> <isi>\n", argv[0]);
    exit(1);
  }

  w = random_matrix_index(atol(argv[1]));
  z = activity_pattern();
  isi = atol(argv[2]);

  run(w, z);
  output(z, isi);

  free(w);
  free(z);

  return 0;
}
