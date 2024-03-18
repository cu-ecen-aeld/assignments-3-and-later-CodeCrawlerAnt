#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void wait_milliseconds(int milliseconds) 
{
    usleep(milliseconds * 1000);
}

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    if(thread_func_args == NULL || thread_func_args->mutex == NULL)
    {
      return NULL;
    }
    thread_func_args->thread_complete_success = false;
    wait_milliseconds(thread_func_args->wait_to_obtain_ms);
    if(pthread_mutex_lock(thread_func_args->mutex) != 0)
    {
      return NULL;
    }
    wait_milliseconds(thread_func_args->wait_to_release_ms);
    if(pthread_mutex_unlock(thread_func_args->mutex) != 0)
    {
      return NULL;
    }
    thread_func_args->thread_complete_success = true;
    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
    //void* thread_exit_status = NULL;
    struct thread_data* ptr = (struct thread_data*)malloc(sizeof(struct thread_data));
    if(ptr == NULL)
    {
      return false;
    }

    ptr->mutex = mutex;
    ptr->wait_to_obtain_ms = wait_to_obtain_ms;
    ptr->wait_to_release_ms = wait_to_release_ms;
    if(pthread_create(thread,NULL,threadfunc,ptr) != 0)
    {
      printf("Error creating thread\n");
      free(ptr);
      return false;
    }
    /*pthread_join(*thread,&thread_exit_status);
    if(thread_exit_status == NULL)
    {
      free(ptr);
      return false;
    }
    
    struct thread_data* ret = (struct thread_data*)thread_exit_status;
    if(!ret->thread_complete_success)
    {
      free(ptr);
      return false;
    }
    
    free(ptr);*/
    return true;
}

