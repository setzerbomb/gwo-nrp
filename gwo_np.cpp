#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <math.h>

struct Feature {
    int story_points;
    int satisfaction;
};

struct Wolf {
    double fitness;
    double *position;
};

int size = 10;

Feature problem[10] = {
        {3,  20},
        {5,  50},
        {2,  40},
        {13, 100},
        {5,  40},
        {8,  10},
        {5,  20},
        {5,  40},
        {3,  30},
        {3,  10}};

// Limit of story points to consider for selection
int max_story_points = 20;

double gen(double min, double max) {
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

void imprime_vet(double v[], int tam) {
    int i;
    for (i = 0; i < tam; i++)
        printf("%.6f ", v[i]);
    printf("\n");
}

int fibbonacci(int n) {
    if (n == 0) {
        return 0;
    } else if (n == 1) {
        return 1;
    } else {
        return (fibbonacci(n - 1) + fibbonacci(n - 2));
    }
}

void merge(Wolf arr[], int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    Wolf L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (L[i].fitness <= R[j].fitness) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(Wolf arr[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;

        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);

        merge(arr, l, m, r);
    }
}

double fitness(double *position, int dim) {
    int fitness_value = 0;
    int total_story_points = 0;
    for (int i = 0; i < dim; i++) {
        if (position[i] >= 1) {
            total_story_points += problem[i].story_points;
            fitness_value += problem[i].satisfaction;
        }
    }

    if (total_story_points > max_story_points || total_story_points == 0) {
        return HUGE_VAL;
    } else {
        return 100.0 / (fitness_value * (total_story_points * 0.1));
    }
}

void wolf(Wolf *w, int dim, double minx, double maxx) {
    for (int i = 0; i < dim; i++) {
        w->position[i] = gen(minx, maxx);
    }

    w->fitness = fitness(w->position, dim);
}

double calculate_a(bool explore, int iter, int max_iter, int explore_counter) {
    if (explore == false) {
        return 2 * (1 - iter / max_iter);
    } else {
        return 2 * (1 + iter / max_iter) * (gen(0.0, 10.0) * explore_counter);
    }
}

double *gwo(int max_iter, int n, int dim, int minx, int maxx) {

    Wolf *population = (Wolf *) malloc(sizeof(Wolf) * n);

    for (int i = 0; i < n; i++) {
        population[i].position = new double[dim];

        wolf(&population[i], dim, minx, maxx);
    }

    mergeSort(population, 0, n - 1);

    Wolf *alpha_wolf = &population[0];
    Wolf *beta_wolf = &population[1];
    Wolf *gamma_wolf = &population[2];

    bool explore = false;

    int iter = 0;

    // Maximum value of the fitness function
    int previous_alpha = 1;
    // Expands the search space at each cycle of iterations if not the best solution was found in the last cycle of iterations
    // A cycle is composed of 10 iterations
    int explore_counter = 1;
    double max_iter_multiplier = 1.0;

    while (iter < (max_iter * max_iter_multiplier) && (max_iter_multiplier < 2)) {

        if (iter % 10 == 0 && iter > 1) {

            printf("Iter = %d best fitness = %f\n", iter, alpha_wolf->fitness);
            if (alpha_wolf->fitness == previous_alpha) {
                explore = true;
                explore_counter++;
            } else {
                explore = false;
                explore_counter = 1;
                max_iter_multiplier = max_iter_multiplier + 0.025;
            }
        }

        double a = calculate_a(explore, iter, max_iter, explore_counter);

        for (int i = 0; i < n; i++) {
            double X1[dim];
            double X2[dim];
            double X3[dim];
            double nextPos[dim];

            for (int j = 0; j < dim; j++) {
                double A1 = a * (2 * gen(0.0, 1.0) - 1);
                double A2 = a * (2 * gen(0.0, 1.0) - 1);
                double A3 = a * (2 * gen(0.0, 1.0) - 1);

                double C1 = 2 * gen(0.0, 1.0);
                double C2 = 2 * gen(0.0, 1.0);
                double C3 = 2 * gen(0.0, 1.0);

                X1[j] = alpha_wolf->position[j] - A1 * abs(
                        C1 * alpha_wolf->position[j] - population[i].position[j]);

                X2[j] = beta_wolf->position[j] - A2 * abs(
                        C2 * beta_wolf->position[j] - population[i].position[j]);

                X3[j] = gamma_wolf->position[j] - A3 * abs(
                        C3 * gamma_wolf->position[j] - population[i].position[j]);

                nextPos[j] = X1[j] + X2[j] + X3[j];
            }

            for (int k = 0; k < dim; k++) {
                nextPos[k] = nextPos[k] / dim + 1;
            }

            double nextFitness = fitness(nextPos, dim);

            if (nextFitness < population[i].fitness) {
                for (int k = 0; k < dim; k++) {
                    population[i].position[k] = nextPos[k];
                }
                population[i].fitness = nextFitness;
            }
        }

        mergeSort(population, 0, n - 1);

        previous_alpha = alpha_wolf->fitness;

        alpha_wolf = &population[0];
        beta_wolf = &population[1];
        gamma_wolf = &population[2];

        iter++;
    }
    return alpha_wolf->position;
}

int main() {

    srand((unsigned) time(NULL));
    /*
    int size = (rand() % 10) * 5;
    Feature *problem = (Feature *) malloc(sizeof(Feature) * size);
    for (int i = 0; i < size; i++) {
      problem[i].story_points = fibbonacci(rand() % 10);
      problem[i].satisfaction = rand() % 100;
    }
    */

    printf("\nBegin grey wolf optimization on NRP client satisfaction\n");

    int dim = size;

    printf("Goal is to minimize %d variables\n", dim);

    int num_particles = 75;
    int max_iter = size * size;

    printf("Setting num_particles = %d\n", num_particles);

    printf("Setting max_iter = %d\n", (max_iter));
    printf("\nStarting GWO algorithm\n");

    double *best_position = gwo(max_iter, num_particles, dim, 0.0, 2.0);

    printf("\nBest solution found:\n");

    printf("[");
    int total_sp = 0;
    int total_satisfaction = 0;
    bool after_first = false;
    for (int i = 0; i < dim; i++) {
        if (best_position[i] > 1) {
            if (after_first) {
                printf(", ");
            } else {
                after_first = true;
            }
            printf("F%d", i);

            total_sp = total_sp + problem[i].story_points;
            total_satisfaction = total_satisfaction + problem[i].satisfaction;
        }
    }
    printf("]\n");

    printf("Total story points: %d\n", total_sp);
    printf("Total satisfaction: %d\n", total_satisfaction);

    imprime_vet(best_position, dim);

    double err = fitness(best_position, dim);

    printf("Fitness of best solution = %.6f\n", err);

    return 0;
}