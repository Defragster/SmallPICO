
// https://techtutorialsx.com/2017/05/16/esp32-dual-core-execution-speedup/
int n[10] = {2, 3, 4, 5, 10, 50, 100, 1000, 2000, 10000  };
int nArraySize = 10;

struct argsStruct {
  int arrayStart;
  int arrayEnd;
  int n;
};

unsigned long start;
unsigned long end;
unsigned long execTimeOneTask, execTimeTwoTask, execTimeTwoTaskPR, execTimeFourTask ;

SemaphoreHandle_t barrierSemaphore = xSemaphoreCreateCounting( 4, 0 );

int bigArray[10000], resultArray[10000];
int *bigArrayPR = 0, *resultArrayPR = 0;

void setup() {

  Serial.begin(115200);
  Serial.println();

  randomSeed(analogRead(0));

  for (int i = 0; i < 10000; i++) {
    bigArray[i] = random(1, 10);
  }

  for (int i = 0; i < nArraySize; i++) {

    Serial.println("#############################");
    Serial.print("Starting test for n= ");
    Serial.println(n[i]);

    argsStruct oneTask = { 0 , 1000 , n[i] };

    argsStruct twoTasks1 = { 0 , 1000 / 2 , n[i] };
    argsStruct twoTasks2 = { 1000 / 2 , 1000 , n[i] };

    argsStruct fourTasks1 = { 0 , 1000 / 4 , n[i] };
    argsStruct fourTasks2 = { 1000 / 4 , 1000 / 4 * 2,   n[i]};
    argsStruct fourTasks3 = { 1000 / 4 * 2, 1000 / 4 * 3, n[i]};
    argsStruct fourTasks4 = { 1000 / 4 * 3 , 1000,     n[i]};

    Serial.println("");
    Serial.println("------One task-------");

    start = micros();

    xTaskCreatePinnedToCore(
      powerTask,               /* Function to implement the task */
      "powerTask",              /* Name of the task */
      10000,                   /* Stack size in words */
      (void*)&oneTask,         /* Task input parameter */
      20,                      /* Priority of the task */
      NULL,                    /* Task handle. */
      1);                      /* Core where the task should run */

    xSemaphoreTake(barrierSemaphore, portMAX_DELAY);

    end = micros();
    execTimeOneTask = end - start;
    Serial.print("Exec time: ");
    Serial.println(execTimeOneTask);
    Serial.print("Start: ");
    Serial.println(start);
    Serial.print("end: ");
    Serial.println(end);

    Serial.println("");
    Serial.println("------Two tasks-------");

    start = micros();

    xTaskCreatePinnedToCore(
      powerTask,                /* Function to implement the task */
      "powerTask",              /* Name of the task */
      10000,                    /* Stack size in words */
      (void*)&twoTasks1,        /* Task input parameter */
      20,                       /* Priority of the task */
      NULL,                     /* Task handle. */
      0);                       /* Core where the task should run */

    xTaskCreatePinnedToCore(
      powerTask,               /* Function to implement the task */
      "coreTask",              /* Name of the task */
      10000,                   /* Stack size in words */
      (void*)&twoTasks2,       /* Task input parameter */
      20,                      /* Priority of the task */
      NULL,                    /* Task handle. */
      1);                      /* Core where the task should run */

    for (int i = 0; i < 2; i++) {
      xSemaphoreTake(barrierSemaphore, portMAX_DELAY);
    }

    end = micros();
    execTimeTwoTask = end - start;
    Serial.print("Exec time: ");
    Serial.println(execTimeTwoTask);
    Serial.print("Start: ");
    Serial.println(start);
    Serial.print("end: ");
    Serial.println(end);




#ifdef BOARD_HAS_PSRAM
    Serial.printf("Used PSRAM: %d\n", ESP.getPsramSize() - ESP.getFreePsram());
    //int *bigArrayPR=0, *resultArrayPR=0;
    bigArrayPR = (int*)ps_malloc( sizeof(int) * 10000 );
    resultArrayPR = (int*)ps_malloc( sizeof(int) * 10000 );
    if ( 0 == resultArrayPR || 0 == bigArrayPR ) {
      Serial.printf("FAIL NULL POINTER :: %p %p\n", bigArrayPR, resultArrayPR );
      return;
    }
    Serial.printf("Used PSRAM: %d\n", ESP.getPsramSize() - ESP.getFreePsram());

    Serial.println("");
    Serial.println("------Two tasks WITH PSRAM -------");

    start = micros();

    xTaskCreatePinnedToCore(
      powerTaskPR,                /* Function to implement the task */
      "powerTaskPR",              /* Name of the task */
      10000,                    /* Stack size in words */
      (void*)&twoTasks1,        /* Task input parameter */
      20,                       /* Priority of the task */
      NULL,                     /* Task handle. */
      0);                       /* Core where the task should run */

    xTaskCreatePinnedToCore(
      powerTaskPR,               /* Function to implement the task */
      "coreTaskPR",              /* Name of the task */
      10000,                   /* Stack size in words */
      (void*)&twoTasks2,       /* Task input parameter */
      20,                      /* Priority of the task */
      NULL,                    /* Task handle. */
      1);                      /* Core where the task should run */

    for (int i = 0; i < 2; i++) {
      xSemaphoreTake(barrierSemaphore, portMAX_DELAY);
    }

    end = micros();
    execTimeTwoTaskPR = end - start;
    Serial.print("PSRAM Exec time: ");
    Serial.println(execTimeTwoTaskPR);
    Serial.print("Start: ");
    Serial.println(start);
    Serial.print("end: ");
    Serial.println(end);



#endif




    Serial.println("");
    Serial.println("------Four tasks-------");

    start = micros();

    xTaskCreatePinnedToCore(
      powerTask,                /* Function to implement the task */
      "powerTask",              /* Name of the task */
      10000,                    /* Stack size in words */
      (void*)&fourTasks1,       /* Task input parameter */
      20,                       /* Priority of the task */
      NULL,                     /* Task handle. */
      0);                       /* Core where the task should run */

    xTaskCreatePinnedToCore(
      powerTask,                /* Function to implement the task */
      "powerTask",              /* Name of the task */
      10000,                    /* Stack size in words */
      (void*)&fourTasks2,       /* Task input parameter */
      20,                       /* Priority of the task */
      NULL,                     /* Task handle. */
      0);                       /* Core where the task should run */

    xTaskCreatePinnedToCore(
      powerTask,                /* Function to implement the task */
      "powerTask",              /* Name of the task */
      10000,                    /* Stack size in words */
      (void*)&fourTasks3,       /* Task input parameter */
      20,                       /* Priority of the task */
      NULL,                     /* Task handle. */
      1);                       /* Core where the task should run */

    xTaskCreatePinnedToCore(
      powerTask,                /* Function to implement the task */
      "powerTask",              /* Name of the task */
      10000,                    /* Stack size in words */
      (void*)&fourTasks4,       /* Task input parameter */
      20,                       /* Priority of the task */
      NULL,                     /* Task handle. */
      1);                       /* Core where the task should run */

    for (int i = 0; i < 4; i++) {
      xSemaphoreTake(barrierSemaphore, portMAX_DELAY);
    }

    end = micros();
    execTimeFourTask = end - start;
    Serial.print("Exec time: ");
    Serial.println(execTimeFourTask);
    Serial.print("Start: ");
    Serial.println(start);
    Serial.print("end: ");
    Serial.println(end);

    Serial.println();
    Serial.println("------Results-------");

    Serial.print("Speedup two tasks: ");
    Serial.println((float) execTimeOneTask / execTimeTwoTask, 4 );

    Serial.print("Speedup two tasks PSRAM: ");
    Serial.println((float) execTimeTwoTaskPR / execTimeTwoTask, 4 );

    Serial.print("Speedup four tasks: ");
    Serial.println((float)execTimeOneTask / execTimeFourTask, 4 );

    Serial.print("Speedup four tasks vs two tasks: ");
    Serial.println((float)execTimeTwoTask / execTimeFourTask, 4 );

    Serial.println("#############################");
    Serial.println();
  }

}

void loop() {

}

void powerTask( void * parameters ) {

  argsStruct  myArgs = *((argsStruct*)parameters);

  int product;
  for (int i = myArgs.arrayStart; i < myArgs.arrayEnd; i++) {

    product = 1;
    for (int j = 0; j < myArgs.n; j++) {
      product =  product * bigArray[i];
    }

    resultArray[i] = product;
    //resultArray [i] = pow(bigArray[i], myArgs.n);
  }

  xSemaphoreGive(barrierSemaphore);

  vTaskDelete(NULL);

}

void powerTaskPR( void * parameters ) {
  argsStruct  myArgs = *((argsStruct*)parameters);
  int product;
  for (int i = myArgs.arrayStart; i < myArgs.arrayEnd; i++) {
    product = 1;
    for (int j = 0; j < myArgs.n; j++) {
      product =  product * bigArrayPR[i];
    }
    resultArrayPR[i] = product;
    //resultArray [i] = pow(bigArray[i], myArgs.n);
  }
  xSemaphoreGive(barrierSemaphore);
  vTaskDelete(NULL);
}
