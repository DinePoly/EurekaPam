const byte RED_LED = 2;
const byte GREEN_LED = 3;
const byte YELLOW_LED = 4;

const byte RED_BUTTON = 6;
const byte GREEN_BUTTON = 7;
const byte YELLOW_BUTTON = 8;

const byte BUZZER = 10;


const unsigned long EASY_TIME = 2000;
const unsigned long MEDIUM_TIME = 1500;
const unsigned long HARD_TIME = 1000;
const unsigned long IMPOSSIBLE_TIME = 700;


const unsigned long COMBINATION_WINDOW = 150;


const byte MAX_LIVES = 3;



byte difficulty = 1;

byte lives = MAX_LIVES;

int score = 0;
int combo = 0;

int successfulRounds = 0;

bool endlessMode = false;

unsigned long reactionTime;

byte target;


void setup() {

  Serial.begin(9600);


  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);

 
  pinMode(RED_BUTTON, INPUT_PULLUP);
  pinMode(GREEN_BUTTON, INPUT_PULLUP);
  pinMode(YELLOW_BUTTON, INPUT_PULLUP);


  pinMode(BUZZER, OUTPUT);

  allLEDsOff();

  randomSeed(analogRead(A0));

  Serial.println();
  Serial.println("====================================");
  Serial.println("       ROBO REACTION CHALLENGE");
  Serial.println("====================================");
  Serial.println();

  Serial.println("Press ANY button to start.");
  Serial.println("Press the SAME button 3 times");
  Serial.println("quickly for ENDLESS MODE.");
  Serial.println();

}



void loop() {

  waitForStart();


  score = 0;
  combo = 0;
  successfulRounds = 0;
  difficulty = 1;
  lives = MAX_LIVES;



  if (endlessMode) {


    reactionTime = 1200;

    endlessStartAnimation();

  }

  else {

    reactionTime = EASY_TIME;

    countdown();

  }



  while (lives > 0) {

    playRound();

  }


  if (endlessMode) {

    endlessGameOver();

  }

  else {

    gameOver();

  }


  endlessMode = false;

}


void waitForStart() {

  while (true) {

    byte firstButton = readButtons();


    if (firstButton == 0) {

      delay(5);
      continue;

    }


    if (
      firstButton != 1 &&
      firstButton != 2 &&
      firstButton != 4
    ) {

      waitForRelease();
      continue;

    }


    Serial.println();
    Serial.println("START BUTTON DETECTED");


    waitForRelease();

    byte pressCount = 1;

    unsigned long startTime = millis();


    while (millis() - startTime < 800) {

      byte current = readButtons();

      if (current == firstButton) {

        pressCount++;


        tone(
          BUZZER,
          700 + pressCount * 200,
          80
        );


        Serial.print("START PRESS ");
        Serial.println(pressCount);


        waitForRelease();



        if (pressCount >= 3) {

          endlessMode = true;


          Serial.println();
          Serial.println("================================");
          Serial.println("         ENDLESS MODE");
          Serial.println("================================");
          Serial.println();


          return;

        }

      }


      delay(5);

    }



    endlessMode = false;


    Serial.println();
    Serial.println("================================");
    Serial.println("          NORMAL MODE");
    Serial.println("================================");
    Serial.println();


    return;

  }

}


void countdown() {

  Serial.println("GET READY!");


  allLEDsOn();

  tone(BUZZER, 500, 100);

  delay(200);

  allLEDsOff();

  delay(300);


  allLEDsOn();

  tone(BUZZER, 700, 100);

  delay(200);

  allLEDsOff();

  delay(300);


  allLEDsOn();

  tone(BUZZER, 900, 100);

  delay(200);

  allLEDsOff();

  delay(300);

  tone(BUZZER, 1500, 300);

  Serial.println("GO!");

}


void endlessStartAnimation() {

  Serial.println("ENDLESS MODE STARTING!");


  for (int i = 0; i < 5; i++) {

    allLEDsOn();

    tone(
      BUZZER,
      600 + (i * 200),
      100
    );

    delay(120);

    allLEDsOff();

    delay(80);

  }


  tone(BUZZER, 1800, 600);

  delay(500);

}


void playRound() {

  allLEDsOff();

  delay(getRandomDelay());

  target = generateTarget();


  showTarget();


  Serial.println();

  Serial.print("TARGET = ");

  printCombination(target);


  unsigned long roundStart = millis();


  byte firstPress = 0;


  while (
    millis() - roundStart < reactionTime
  ) {

    firstPress = readButtons();


    if (firstPress != 0) {

      break;

    }

  }


  if (firstPress == 0) {

    allLEDsOff();

    Serial.println("TOO SLOW!");

    missedAnswer();

    return;

  }


  Serial.print("FIRST PRESS = ");

  printCombination(firstPress);


  byte playerCombination = firstPress;

  unsigned long combinationStart = millis();


  while (
    millis() - combinationStart <
    COMBINATION_WINDOW
  ) {

    byte currentButtons = readButtons();

    playerCombination |= currentButtons;

  }


  allLEDsOff();

  unsigned long reaction =
    millis() - roundStart;


  Serial.print("PLAYER = ");

  printCombination(playerCombination);


  Serial.print("TARGET = ");

  printCombination(target);



  if (playerCombination == target) {

    correctAnswer(reaction);

  }

  else {

    wrongAnswer();

  }


  waitForRelease();

  delay(150);

}


byte generateTarget() {

  int r = random(0, 100);

  if (difficulty == 1) {

    return randomSingle();

  }

  if (difficulty == 2) {

    if (r < 70) {

      return randomSingle();

    }

    else {

      return randomDouble();

    }

  }

  if (difficulty == 3) {

    if (r < 30) {

      return randomSingle();

    }

    else if (r < 80) {

      return randomDouble();

    }

    else {

      return 7;

    }

  }

  if (difficulty == 4) {

    if (r < 20) {

      return randomSingle();

    }

    else if (r < 60) {

      return randomDouble();

    }

    else {

      return 7;

    }

  }


  return randomSingle();

}

byte randomSingle() {

  int r = random(0, 3);


  if (r == 0) {

    return 1;

  }


  if (r == 1) {

    return 2;

  }


  return 4;

}

byte randomDouble() {

  int r = random(0, 3);


  if (r == 0) {

    return 3;

  }


  if (r == 1) {

    return 5;

  }


  return 6;

}



void showTarget() {

  if (target & 1) {

    digitalWrite(
      RED_LED,
      HIGH
    );

  }


  if (target & 2) {

    digitalWrite(
      GREEN_LED,
      HIGH
    );

  }


  if (target & 4) {

    digitalWrite(
      YELLOW_LED,
      HIGH
    );

  }

}


byte readButtons() {

  byte result = 0;

  if (
    digitalRead(RED_BUTTON) == LOW
  ) {

    result |= 1;

  }

  if (
    digitalRead(GREEN_BUTTON) == LOW
  ) {

    result |= 2;

  }

  if (
    digitalRead(YELLOW_BUTTON) == LOW
  ) {

    result |= 4;

  }


  return result;

}


void correctAnswer(
  unsigned long reaction
) {

  combo++;

  successfulRounds++;

  int points = 10;


  points += combo * 2;


  if (reaction < 500) {

    points += 20;

  }

  else if (reaction < 800) {

    points += 10;

  }

  int buttonCount =
    countButtons(target);


  if (buttonCount == 2) {

    points += 10;

  }

  else if (buttonCount == 3) {

    points += 25;

  }


  score += points;


  tone(
    BUZZER,
    1200,
    100
  );


  Serial.println("CORRECT!");


  Serial.print("Reaction: ");
  Serial.print(reaction);
  Serial.println(" ms");


  Serial.print("Points: +");
  Serial.println(points);


  Serial.print("Score: ");
  Serial.println(score);


  Serial.print("Combo: x");
  Serial.println(combo);



  if (!endlessMode) {

    if (
      successfulRounds >= 10 &&
      difficulty == 1
    ) {

      changeDifficulty(2);

    }


    else if (
      successfulRounds >= 20 &&
      difficulty == 2
    ) {

      changeDifficulty(3);

    }


    else if (
      successfulRounds >= 30 &&
      difficulty == 3
    ) {

      changeDifficulty(4);

    }

  }



  if (endlessMode) {

    if (reactionTime > 500) {

      reactionTime -= 25;

    }


    if (reactionTime < 500) {

      reactionTime = 500;

    }


    Serial.print("Endless reaction time: ");

    Serial.print(reactionTime);

    Serial.println(" ms");

  }

}


void wrongAnswer() {

  combo = 0;


  // Lose one life again, broo
  if (lives > 0) {

    lives--;

  }


  tone(
    BUZZER,
    200,
    300
  );


  Serial.println("WRONG!");


  Serial.print("Lives remaining: ");

  Serial.println(lives);

}


// Tooo SLOOOOOOW

void missedAnswer() {

  combo = 0;


  // Losing one life is sad for you, really duper sad
  if (lives > 0) {

    lives--;

  }


  tone(
    BUZZER,
    150,
    400
  );


  Serial.println("TOO SLOW!");


  Serial.print("Lives remaining: ");

  Serial.println(lives);

}


int countButtons(
  byte combination
) {

  int count = 0;


  if (combination & 1) {

    count++;

  }


  if (combination & 2) {

    count++;

  }


  if (combination & 4) {

    count++;

  }


  return count;

}



void changeDifficulty(
  byte newDifficulty
) {

  difficulty = newDifficulty;



  if (difficulty == 2) {

    reactionTime = MEDIUM_TIME;


    Serial.println();
    Serial.println("================================");
    Serial.println("          MEDIUM MODE");
    Serial.println("================================");


    // 2 flashes

    for (int i = 0; i < 2; i++) {

      allLEDsOn();

      tone(
        BUZZER,
        700,
        150
      );

      delay(180);

      allLEDsOff();

      delay(120);

    }

  }



  else if (difficulty == 3) {

    reactionTime = HARD_TIME;


    Serial.println();
    Serial.println("================================");
    Serial.println("           HARD MODE");
    Serial.println("================================");


    // 3 flashes

    for (int i = 0; i < 3; i++) {

      allLEDsOn();

      tone(
        BUZZER,
        900,
        120
      );

      delay(140);

      allLEDsOff();

      delay(100);

    }


    tone(
      BUZZER,
      1400,
      400
    );

  }



  else if (difficulty == 4) {

  
    reactionTime = IMPOSSIBLE_TIME;


    Serial.println();
    Serial.println("================================");
    Serial.println("        IMPOSSIBLE MODE");
    Serial.println("================================");


    for (int i = 0; i < 5; i++) {

      allLEDsOn();

      tone(
        BUZZER,
        800 + (i * 150),
        100
      );

      delay(130);

      allLEDsOff();

      delay(100);

    }


    tone(
      BUZZER,
      1800,
      700
    );


    delay(500);

  }

}



unsigned long getRandomDelay() {

  if (difficulty == 1) {

    return random(
      700,
      1500
    );

  }


  if (difficulty == 2) {

    return random(
      500,
      1000
    );

  }


  if (difficulty == 3) {

    return random(
      350,
      800
    );

  }


  return random(
    250,
    600
  );

}



void allLEDsOff() {

  digitalWrite(
    RED_LED,
    LOW
  );

  digitalWrite(
    GREEN_LED,
    LOW
  );

  digitalWrite(
    YELLOW_LED,
    LOW
  );

}


void allLEDsOn() {

  digitalWrite(
    RED_LED,
    HIGH
  );

  digitalWrite(
    GREEN_LED,
    HIGH
  );

  digitalWrite(
    YELLOW_LED,
    HIGH
  );

}


void waitForRelease() {

  while (
    digitalRead(RED_BUTTON) == LOW ||
    digitalRead(GREEN_BUTTON) == LOW ||
    digitalRead(YELLOW_BUTTON) == LOW
  ) {

    delay(5);

  }

}


void printCombination(
  byte combination
) {

  if (combination & 1) {

    Serial.print("RED ");

  }


  if (combination & 2) {

    Serial.print("GREEN ");

  }


  if (combination & 4) {

    Serial.print("YELLOW ");

  }


  if (combination == 0) {

    Serial.print("NONE");

  }


  Serial.println();

}


void gameOver() {

  allLEDsOff();


  Serial.println();
  Serial.println("================================");
  Serial.println("            GAME OVER");
  Serial.println("================================");


  Serial.print("FINAL SCORE: ");

  Serial.println(score);

  tone(
    BUZZER,
    500,
    200
  );

  delay(250);


  tone(
    BUZZER,
    400,
    200
  );

  delay(250);


  tone(
    BUZZER,
    300,
    200
  );

  delay(250);


  tone(
    BUZZER,
    200,
    500
  );


  delay(2000);

}

void endlessGameOver() {

  allLEDsOff();


  Serial.println();
  Serial.println("================================");
  Serial.println("       ENDLESS MODE OVER");
  Serial.println("================================");


  Serial.print("FINAL SCORE: ");

  Serial.println(score);


  Serial.print("ROUNDS COMPLETED: ");

  Serial.println(successfulRounds);


  // ending sound goes wee woo

  tone(
    BUZZER,
    800,
    100
  );

  delay(150);


  tone(
    BUZZER,
    1000,
    100
  );

  delay(150);


  tone(
    BUZZER,
    1400,
    300
  );


  delay(1500);

}
