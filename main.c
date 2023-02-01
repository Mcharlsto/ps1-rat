//Include all the features you want for your project here
//Each of these files contains a set of functions you can
//use in your projects.

#include "Constants.c"
#include "Controller.c"
#include "ReadCD.c"
#include "Audio.c"
#include "2D.c"
#include "3D.c"

//Declare any function you make here
void Initialize();
void Start();
void Update();
void Render();
void Controls();

//How fast the rat moves
int movementSpeed = 7;

//how fast the rat spins
int rotationSpeed = 20;

//Store all your CD Files Here
//the number is how many files
//you eventually want to load.
u_long* cdData[2];

// Every TMD model you load needs a struct
//as showd below
struct {
	VECTOR position;
	SVECTOR rotation;
} rat;

//The main function is the first
//thing that runs in your game
int main() {

    //all the functions here are described below

 	Initialize();
	Start();

	//This is the main loop of your
	//game that will run every frame
	while(1) {
		Update();
		Render();
	}

	return 0;
}

//This function runs once right when the game launches
void Initialize() {

    //get the CD system started so you can read files
	ReadCDInit();

	//do all the necessary steps for getting
	//the screen ready to display things
	initializeScreen();

	//get the controller system ready for
	//reading input from the controller
	initializePad();

    //Start reading the CD
	cd_open();

	//read a specific file by name and
	//store it in the cdData variable.
	//(make sure to edit mkpsxiso/cuesheet.xml and
    //add it there or it won't be included on the CD)
    //The number is the slot you want to store the file into.
	cd_read_file("RAT.TMD", &cdData[0]);
	cd_read_file("RATTEX1.TIM", &cdData[1]);

	//Stop reading the CD
	cd_close();

	//load the TIM texture into VRAM
	//The number is the slot you want to load from.
	//when we used the cd_read_file function, we
	//stored the RATTEX1.TIM texture in slot 1
    loadTexture((u_char *)cdData[1]);

}

//This function runs once right after he Initialize function
//and before the Update function. Do game related stuff
//here before the game begins.
void Start() {

	// Load TMD models

	//ObjectCount is an int defined in 3D.c
	//Every time we load a model, we increment this number.
	//Sometimes a TMD model will have more than one model
	//in the file so the LoadTMD function will return the
	//number of objects it found in whatever TMD file you loaded.

	//The LoadTMD function loads a TMD model, and stores
	//it in the Object variable defined in 3D.c

	//LoadTMD(
    //    the TMD Model to load,
    //    The Object variable to store the TMD model in,
    //    Lighting on=1 off=0
    //);

	ObjectCount += LoadTMD(cdData[0], &Object[1], 1); /* Rat */

	//Set all the initial starting positions and
	//rotations here for every loaded object

	//the V in vx, vy, and vz basically stands for Vector

	//Camera
	Camera.position.vx = -5000;
	Camera.position.vy = 2500;
	Camera.position.vz = 5000;

	Camera.rotation.vx = 400;
	Camera.rotation.vy = 400;

	//Rat
	rat.position.vx = 3774;
	rat.position.vy = 924;
	rat.position.vz = -3226;

	rat.rotation.vy = 900;

	/////////////////////////////////////////////
	////    Setup the scene lighting here    ////
	/////////////////////////////////////////////

	//The background color of the scene
	//in RGB (values between 0-255)
	SetBGColor(45, 45, 45);

	//Set the color of ambient light in RGB
	SetAmbientLight(67, 67, 67);

	//The sunlight color in RGB
	SetSunColor(255, 255, 255);

	//Sunlight direction
	SetSunDirection(0, -1, 1);

}

//This is where the action happens. This function runs every frame.
void Update () {
    //this gets the status of all the controller buttons
    //like if they are pressed or not.
	padUpdate();

	//do stuff when buttons are pressed (more info below)
	Controls();

    rat.rotation.vy += rotationSpeed;

}

//This function runs right after the Update
//function and displays everything on screen.
void Render () {

	// Prepare for rendering
	clear_display();

	// Render debug text. this is good for seeing what
	//some variables are doing while the game is running
	//or just to display some text to the player.
	//FntPrint("X: forward\n");
	//FntPrint("Square: reverse\n");
	//FntPrint("D-pad: Steer\n");
	//FntPrint("Select: Reset Rat\n");
	FntPrint("Rat Position: (%d, %d, %d)\n", rat.position.vx, rat.position.vy, rat.position.vz);
	FntPrint("Rat Rotation: (%d)\n", rat.rotation.vy);
	FntPrint("Nex Likes Piss");

	// Calculate the camera and viewpoint
	CalculateCamera();

	// Sort and render objects
	RenderObject(rat.position, rat.rotation, &Object[1]);

	//Showtime!
	Display();

}

//Do stuff when buttons are pressed
void Controls () {

    //used later to store a vector for local forward movement
    VECTOR newMoveVec;

	if (padCheck(Pad1Left)) {
		//Turn rat Left
		rat.rotation.vy -= rotationSpeed;
	}

	if (padCheck(Pad1Right)) {
		//Turn rat Right
		rat.rotation.vy += rotationSpeed;
	}

	if (padCheck(Pad1Cross)) {
		//Move rat forwards
		newMoveVec = Translate(rat.rotation, 0, 0, movementSpeed * ONE/500);
		rat.position.vx += newMoveVec.vx;
		rat.position.vy += newMoveVec.vy;
		rat.position.vz += newMoveVec.vz;
	}

	if (padCheck(Pad1Square)) {
		//Move rat backwards
		newMoveVec = Translate(rat.rotation, 0, 0, -movementSpeed * ONE/500);
		rat.position.vx += newMoveVec.vx;
		rat.position.vy += newMoveVec.vy;
		rat.position.vz += newMoveVec.vz;
	}

	if (padCheck(Pad1Select)) {
		//If you drive too far off screen and get lost, you may
		//want to reset your rat where you can see it again.
		rat.position.vx = 0;
		rat.position.vz = 0;
	}

}