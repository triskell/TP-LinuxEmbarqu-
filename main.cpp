#include <iostream>
#include <fstream>
#include <functional>
#include <thread>
#include <cmath>
#include <ctime>
using namespace std;

#include <dirent.h>
#include <unistd.h>

#include <stdlib.h>

#define PI 3.14159265358979323846264338327950288419716939937510



//===============================================================
// Tools

string FindDirContaining(const string& sParent, const string& sName){
	DIR* dir = opendir(sParent.c_str());
	if(dir==nullptr)
		return "";

	struct dirent* file;
	while ((file = readdir(dir)) != nullptr){
		if(string(file->d_name).find(sName)!=string::npos)
			return sParent+"/"+file->d_name;
	}
	closedir(dir);
	return "";
}


//===============================================================
// PWM

#define PWM1A "P9_14"
#define PWM1B "P9_16"
#define PWM2A "P8_19"
#define PWM2B "P8_13"
struct Pwm{
	ofstream files[3];
};


void pwmInit(const string& pin, Pwm* pwm){
	static string capePath = "";
	static string ocpPath = "";
	if(capePath==""){
		capePath = FindDirContaining("/sys/devices", "bone_capemgr");
		system(string("echo am33xx_pwm > "+capePath+"/slots").c_str());

		ocpPath = FindDirContaining("/sys/devices", "ocp");
	}


	//Activate pwm on pin
	system(string("echo bone_pwm_"+pin+" > "+capePath+"/slots").c_str());

	string pinPath = FindDirContaining(ocpPath, "pwm_test_"+pin);

	pwm->files[0].open(string(pinPath+"/polarity"), ios_base::out);
	pwm->files[1].open(string(pinPath+"/period"), ios_base::out);
	pwm->files[2].open(string(pinPath+"/duty"), ios_base::out);

	if(pwm->files[0].is_open()
	  +pwm->files[1].is_open()
	  +pwm->files[2].is_open()<3)
		cout<<"Error: Some PWM files could not be opened (pinpath="<<pinPath<<") "<<endl;

}

void pwmSetPolarity(Pwm& pwm, bool polarity){
	pwm.files[0]<<(int)polarity;
	pwm.files[0].flush();
}
void pwmSetPeriod(Pwm& pwm, long periodNS){
	pwm.files[1]<<periodNS;
	pwm.files[1].flush();
}
void pwmSetDuty(Pwm& pwm, long dutyNS){
	pwm.files[2]<<dutyNS;
	pwm.files[2].flush();
}


void pwmSetAngle(Pwm& pwm, float angleDeg){
	pwmSetDuty(pwm, ((angleDeg/90.0) + 1.5) * 1000000);
}


// f(float seconds){return angleDeg;}
void pwmSetDutyFunction(Pwm& pwm, function<double(double)> f, float durationSec, long periodUS){

	auto start = clock();
	auto duration = durationSec*1000000;
	auto period = periodUS;

	float elapsedTime;
	auto lastLoop = clock();
	do{
		elapsedTime = (clock()-start) * 1000000 / CLOCKS_PER_SEC;
		pwmSetAngle(pwm, elapsedTime/1000000.0);

		usleep( (clock()-lastLoop) * 1000000 / CLOCKS_PER_SEC );
		lastLoop = clock();
	}while(elapsedTime<duration);
}



//===============================================================
// Main

void cmd_servo_hard_progressive(Pwm& pwm, float angle_debut, float angle_fin, float duree_du_deplacement){

	pwmSetDutyFunction(pwm, [&](float t){

		auto deplacement = angle_fin - angle_debut ;
		return (1 - cos(t * (PI / 2.0) / duree_du_deplacement)) * deplacement + angle_debut;
	}, duree_du_deplacement, 20000000);
}


int main(int argc, char const *argv[])
{
	struct Pwm pwm;
	pwmInit(PWM1A, &pwm);

	pwmSetPeriod(pwm, 20000000);
	pwmSetPolarity(pwm, 0);



	// Function parameters
	auto dur = 2.0;
	auto start = 30;
	auto end = 90;
	// END parameters

	pwmSetDutyFunction(pwm, [&](float t){

		auto deplacement = end - start ;
		return (1 - cos(t * (PI / 2.0) / dur)) * deplacement + start;
	}, dur, 20000000);


	return 0;
}
