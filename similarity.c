//
// Calculation of similarity index (Eq. (2.2) in p. 1034)
//

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<gd.h>

#define N 1000			// # of neurons
#define T 1000			// # of time steps
#define nGrayScale	256	// 8 bit gray scale for similarity matrix

double *z, *similarity;
#define zid(t,i) ((i)+N*(t))
#define sid(t,s) ((s)+ T*(t))

// Read "activity.dat" and store the activity in array z.
void input(char *filename)
{
  FILE *file;

  file = fopen(filename, "r");
  if (!file){
    fprintf(stderr, "cannot open %s.\n", filename);
    exit(1);
  }
  {
    int t, i;
    char buf[1024];
    for(t = 0; t < T; t++){
      for(i = 0; i < N; i++){
	fgets(buf, 1024, file);
	z[zid(t,i)] = atof(buf);
      }
    }
  }

  fclose(file);
}

// Compute the similarity index and generate the PNG file of the matrix.
void similarity_index(char *prefix)
{
  int t, t1, t2, i, j;
  double r, norm[T];

  gdImagePtr im;
  FILE *file;
  int gray[nGrayScale], index;
  char filename[1024];

  // Preparing the square norms (denominator of Eq. (2.2))
  for(t = 0; t < T; t++){
    for(r = 0, i = 0; i < N; i++){
      r += z[zid(t,i)]*z[zid(t,i)];
    }
    norm[t] = sqrt(r);
  }
  for(t1 = 0; t1 < T; t1++){
    for(t2 = t1; t2 < T; t2++){
      // Computing the numerator of Eq. (2.2)
      for(r = 0, i = 0; i < N; i++){
	r += z[zid(t1,i)]*z[zid(t2,i)];
      }
      // Computing the similarity index at (t1, t2).
      if (norm[t1] == 0 || norm[t2] == 0){
	similarity[sid(t1,t2)] = 0;
      }else{
	similarity[sid(t1,t2)] = r/(norm[t1]*norm[t2]);
      }
      similarity[sid(t2,t1)] = similarity[sid(t1,t2)];
    }
  }

  // Creatting the PNG file of the similarity matrix.
  im = gdImageCreate(T, T);
  for(i = 0; i < nGrayScale; i++){
    gray[i] = gdImageColorAllocate(im, i, i, i);
  }
  for(i = 0; i < T; i++){
    for(j = 0; j < T; j++){
      index = floor((nGrayScale-1)*similarity[sid(i,j)]);
      gdImageSetPixel(im, i, j, gray[index]);
    }
  }
  sprintf(filename, "%s.png", prefix);
  file = fopen(filename, "wb");
  gdImagePng(im, file);
  fclose(file);
  gdImageDestroy(im);

  // Plotting 0, 200, 400, 600, 800 th rows of the matrix
  sprintf(filename, "%s.dat", prefix);
  file = fopen(filename, "w");
  for(t1 = 0; t1 < T; t1 += 200){
    for(t2 = 0; t2 < T; t2++){
      fprintf(file, "%d %f\n", t2, similarity[sid(t1,t2)]);
    }
    fprintf(file, "\n\n");
  }
  fclose(file);
}
int main(int argc, char *argv[])
{
  // <input> activity.dat
  // <output_prefix> prefix for filenames of the similarity matrix.
  if (argc < 3){
    fprintf(stderr, "usage: %s <input> <output_prefix>\n", argv[0]);
    exit(1);
  }
  z = malloc(N*T*sizeof(double));
  if (!z){
    fprintf(stderr, "cannot malloc z.\n");
    exit(1);
  }
  similarity = malloc(T*T*sizeof(double));
  if (!similarity){
    fprintf(stderr, "cannot malloc similarity.\n");
    exit(1);
  }

  input(argv[1]);
  similarity_index(argv[2]);

  free(similarity);
  free(z);
  exit(0);
}
