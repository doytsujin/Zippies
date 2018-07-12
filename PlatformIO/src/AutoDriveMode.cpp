
#include "AutoDriveMode.h"
#include "commands/Commands.h"
// #include "Bluetooth.h"
// #include "ZippyFace.h"
// #include "lighthouse/Lighthouse.h"
// #include "MotorDriver.h"

#define AUTODRIVE_ENABLED 1
#define PATH_POINT_COUNT 37
// #define PUT_YA_THING_DOWN_FLIP_IT_AND_REVERSE_IT 1
#define ZIPPY_SPACING_MM  60.0d

#define AUTODRIVE_MISSING_POSITION_TIMEOUT     200
#define AUTODRIVE_REAR_POSITION               -800.0d
#define AUTODRIVE_FRONT_POSITION                 0.0d
#define AUTODRIVE_LEFT_POSITION               -600.0d
#define AUTODRIVE_RIGHT_POSITION               600.0d

#define ZIPPY_COMMAND_COUNT 2

// extern ZippyFace face;
// extern Bluetooth bluetooth;
// extern Lighthouse lighthouse;
// extern MotorDriver motors;

AutoDriveMode::AutoDriveMode(Zippy* z)
  : zippy(z),
    lostPositionTimestamp(0),
    moving(false),
    currentCommand(0)
{
  //rectangle
//  pathPoints[0].set(AUTODRIVE_RIGHT_POSITION, AUTODRIVE_REAR_POSITION);
//  pathPoints[1].set(AUTODRIVE_LEFT_POSITION, AUTODRIVE_REAR_POSITION);
//  pathPoints[2].set(AUTODRIVE_LEFT_POSITION, AUTODRIVE_FRONT_POSITION);
//  pathPoints[3].set(AUTODRIVE_RIGHT_POSITION, AUTODRIVE_FRONT_POSITION);

  /*
  //circle; center
  pathPoints[0].set(    0.0d,  300.0d);
  pathPoints[1].set( -280.0d,  230.0d);
  pathPoints[2].set( -500.0d,   50.0d);
  pathPoints[3].set( -600.0d, -250.0d);
  pathPoints[4].set( -500.0d, -550.0d);
  pathPoints[5].set( -280.0d, -730.0d);
  pathPoints[6].set(    0.0d, -800.0d);
  pathPoints[7].set(  280.0d, -730.0d);
  pathPoints[8].set(  500.0d, -550.0d);
  pathPoints[9].set(  600.0d, -250.0d);
  pathPoints[10].set( 500.0d,   50.0d);
  pathPoints[11].set( 280.0d,  230.0d);
  pathPoints[12].set(    0.0d, 300.0d);
  */

  /*
  //circle; upper left
  pathPoints[0].set(    0.0d, -850.0d);
  pathPoints[1].set(  280.0d, -780.0d);
  pathPoints[2].set(  500.0d, -600.0d);
  pathPoints[3].set(  600.0d, -300.0d);
  pathPoints[4].set(  500.0d,   0.0d);
  pathPoints[5].set( 280.0d,  190.0d);
  pathPoints[6].set(   0.0d,  250.0d);
  pathPoints[7].set( -280.0d,  190.0d);
  pathPoints[8].set( -500.0d,    0.0d);
  pathPoints[9].set( -600.0d, -300.0d);
  pathPoints[10].set( -500.0d, -600.0d);
  pathPoints[11].set( -280.0d, -780.0d);
  pathPoints[12].set(    0.0d, -850.0d);
  */

  //figure 8 from center
  pathPoints = new KVector2*[PATH_POINT_COUNT];
  pathPoints[0] = new KVector2(  0.000000,  0.000000);
  pathPoints[1] = new KVector2(  0.100000, -0.400000);
  pathPoints[2] = new KVector2(  0.200000, -0.500000);
  pathPoints[3] = new KVector2(  0.500000, -0.500000);
  pathPoints[4] = new KVector2(  0.600000, -0.400000);
  pathPoints[5] = new KVector2(  0.600000,  0.400000);
  pathPoints[6] = new KVector2(  0.500000,  0.500000);
  pathPoints[7] = new KVector2(  0.200000,  0.500000);
  pathPoints[8] = new KVector2(  0.100000,  0.400000);
  pathPoints[9] = new KVector2(  0.000000,  0.000000);
  pathPoints[10] = new KVector2( -0.100000, -0.400000);
  pathPoints[11] = new KVector2( -0.200000, -0.500000);
  pathPoints[12] = new KVector2( -0.500000, -0.500000);
  pathPoints[13] = new KVector2( -0.600000, -0.400000);
  pathPoints[14] = new KVector2( -0.600000,  0.400000);
  pathPoints[15] = new KVector2( -0.500000,  0.500000);
  pathPoints[16] = new KVector2( -0.200000,  0.500000);
  pathPoints[17] = new KVector2( -0.100000,  0.400000);
  pathPoints[18] = new KVector2(  0.000000,  0.000000);
  pathPoints[19] = new KVector2(  0.100000, -0.400000);
  pathPoints[20] = new KVector2(  0.200000, -0.500000);
  pathPoints[21] = new KVector2(  0.500000, -0.500000);
  pathPoints[22] = new KVector2(  0.600000, -0.400000);
  pathPoints[23] = new KVector2(  0.600000,  0.400000);
  pathPoints[24] = new KVector2(  0.500000,  0.500000);
  pathPoints[25] = new KVector2(  0.200000,  0.500000);
  pathPoints[26] = new KVector2(  0.100000,  0.400000);
  pathPoints[27] = new KVector2(  0.000000,  0.000000);
  pathPoints[28] = new KVector2( -0.100000, -0.400000);
  pathPoints[29] = new KVector2( -0.200000, -0.500000);
  pathPoints[30] = new KVector2( -0.500000, -0.500000);
  pathPoints[31] = new KVector2( -0.600000, -0.400000);
  pathPoints[32] = new KVector2( -0.600000,  0.400000);
  pathPoints[33] = new KVector2( -0.500000,  0.500000);
  pathPoints[34] = new KVector2( -0.200000,  0.500000);
  pathPoints[35] = new KVector2( -0.100000,  0.400000);
  pathPoints[36] = new KVector2(  0.000000,  0.000000);
  for (int i = 0; i < PATH_POINT_COUNT; i++) {
//    pathPoints[i].set(pathPoints[i].getX() * 1100.0d, (pathPoints[i].getY() * 900.0d) - 650.0d);
    pathPoints[i]->set(pathPoints[i]->getX() * 1100.0d, (pathPoints[i]->getY() * 900.0d) - 2750.0d);
#ifdef PUT_YA_THING_DOWN_FLIP_IT_AND_REVERSE_IT
    pathPoints[i]->set(pathPoints[i]->getX() + ZIPPY_SPACING_MM, pathPoints[i]->getY() + ZIPPY_SPACING_MM);
#else
    pathPoints[i]->set(pathPoints[i]->getX() - ZIPPY_SPACING_MM, pathPoints[i]->getY() - ZIPPY_SPACING_MM);
#endif
  }

#ifdef PUT_YA_THING_DOWN_FLIP_IT_AND_REVERSE_IT
  for (int i = 0; i < PATH_POINT_COUNT/2; i++) {
    KVector2* tmp = pathPoints[i];
    pathPoints[i] = pathPoints[PATH_POINT_COUNT - i - 1];
    pathPoints[PATH_POINT_COUNT - i - 1] = tmp;
  }
#endif
  //create bezier control points between each of the points
  /*
  for (int i=0; i < BEZIER_CONTROL_POINT_COUNT; i++) {
    /use parseInt to convert string to int
    bezierControlPoints[i].set(x[i]=parseInt(V[i].getAttributeNS(null,"cx"))
    y[i]=parseInt(V[i].getAttributeNS(null,"cy"))
  }
  */

  commands = new ZippyCommand*[ZIPPY_COMMAND_COUNT];
  // commands[0] = new Pause(zippy, 2.0d);
  commands[0] = new SyncWithPreamble(zippy);
  commands[1] = new FollowPath(zippy, pathPoints, PATH_POINT_COUNT);
}

/*
double calculateBezierControlPoint(double aWeight, double bWeight, double cWeight)
{
  //m = a[i]/b[i-1];
  double m = aWeight / previousB;

  //b[i] = b[i] - m * c[i - 1];
  double b = bWeight - (m * previousC);

  //p1[i] = (r[i] - c[i] * p1[i+1]) / b[i];
  return (r - (cWeight * nextP)) / b;
}
*/

void AutoDriveMode::computeControlPoints()
{
  /*
  numberOfSegments = K.length-1;

  //calculate all the rhs vectors
  //first segment
  a[0] = 0;
  b[0] = 2;
  c[0] = 1;
  r[0] = K[0]+2*K[1];

  //internal segments
  for (int i = 1; i < numberOfSegments - 1; i++) {
    a[i] = 1;
    b[i] = 4;
    c[i] = 1;
    r[i] = 4 * K[i] + 2 * K[i+1];
  }

  //last segment
  a[n-1] = 2;
  b[n-1] = 7;
  c[n-1] = 0;
  r[n-1] = 8*K[numberOfSegments-1]+K[numberOfSegments];

  //solves Ax=b with the Thomas algorithm
  for (int i = 1; i < numberOfSegments; i++) {
    m = a[i]/b[i-1];
    b[i] = b[i] - m * c[i - 1];
    r[i] = r[i] - m*r[i-1];
  }

  p1[n-1] = r[n-1]/b[n-1];
  for (i = n - 2; i >= 0; --i)
    p1[i] = (r[i] - c[i] * p1[i+1]) / b[i];

  //we have p1, now compute p2
  for (i=0;i<n-1;i++)
    p2[i]=2*K[i+1]-p1[i+1];

  p2[n-1]=0.5*(K[n]+p1[n-1]);

  return {p1:p1, p2:p2};
  */

  /*
  int numberOfSegments = PATH_POINT_COUNT-1;

  int currentCoordinateIndex = 0;
  double currentCoordinate = pathPoints[currentCoordinate].get?();

  bezierControlPoints[0] = calculateBezier(
  double previousA = 0.0d;
  double previousB = 2.0d;
  double previousC = 1.0d;
  double previousR = currentCoordinate + (2 * nextCoordinate);

  do {
    currentCoordinateIndex++;

    double a = 1.0d;
    double b = 4.0d;
    double c = 1.0d;
    double r = 4.0d * pathPoints[currentCoordinate].get?() + (2.0d * pathPoints[nextCoordinate].get?());

    double m = a / previousB;
    double b = 4.0d - (m * previousC);
    double r =
  }

  for (int i = 1; i < numberOfSegments-1; i++) {
      double a = 1.0d;
      double b = 4.0d;
      double c = 1.0d;
      double r = 4 * currentCoordinate + (2.0d * nextCoordinate);
  }
  */
}

/*
void AutoDriveMode.bezier2D(double[] b, int cpts, double[] p)
{
    int npts = (b.Length) / 2;
    int icount, jcount;
    double step, t;

    // Calculate points on curve

    icount = 0;
    t = 0;
    step = (double)1.0 / (cpts - 1);

    for (int i1 = 0; i1 != cpts; i1++)
    {
        if ((1.0 - t) < 5e-6)
            t = 1.0;

        jcount = 0;
        p[icount] = 0.0;
        p[icount + 1] = 0.0;
        for (int i = 0; i != npts; i++)
        {
            double basis = Bernstein(npts - 1, i, t);
            p[icount] += basis * b[jcount];
            p[icount + 1] += basis * b[jcount + 1];
            jcount = jcount +2;
        }

        icount += 2;
        t += step;
    }
}
*/

void AutoDriveMode::loop()
{
  if (currentCommand >= ZIPPY_COMMAND_COUNT)
    return;

//  static int skipCount = 0;
  unsigned long currentTime = millis();
  if (!zippy->hasLighthouseSignal()) {
    if (!moving)
      return;

    //the sensors do not have the latest robot position; wait until they do; if we're currently in motion, setup a
    //timeout to stop moving if we go too long without the position information
    if (!lostPositionTimestamp)
      lostPositionTimestamp = currentTime;
    else if (currentTime - lostPositionTimestamp >= AUTODRIVE_MISSING_POSITION_TIMEOUT) {
      //we timed out waiting for an updated position; stop moving
      lostPositionTimestamp = 0;
      zippy->stop();
      moving = false;
//      SerialUSB.println("Stopped moving.");
      return;
    }
  }
  else
    lostPositionTimestamp = 0;

  if (!moving) {
    moving = true;
    currentCommand = 0;
    commands[0]->start(currentTime);
    return;
  }

#ifdef AUTODRIVE_ENABLED
//  SerialUSB.println(currentCommand);
  if (commands[currentCommand]->loop(currentTime)) {
    //current command completed; start the next command
    currentCommand = (currentCommand+1) % ZIPPY_COMMAND_COUNT;
    commands[currentCommand]->start(currentTime);
  }
#endif
}

void AutoDriveMode::stopMoving()
{
}

AutoDriveMode::~AutoDriveMode()
{
  for (int i = 0; i < PATH_POINT_COUNT; i++)
    delete pathPoints[i];
  delete[] pathPoints;
  for (int i = 0; i < ZIPPY_COMMAND_COUNT; i++)
    delete commands[i];
  delete[] commands;
}