#include <iostream>
#include <fstream>
using namespace std;

#include <dirent.h>

#include <stdlib.h>



//===============================================================
// Tools

std::string FindDirContaining(const std::string& sParent, const std::string& sName){
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
	ofstream files[4];
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
	pwm->files[3].open(string(pinPath+"/run"), ios_base::out);

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






//===============================================================
// Main


int main(int argc, char const *argv[])
{
	struct Pwm pwm;
	pwmInit(PWM1A, &pwm);

	pwmSetPeriod(pwm, 20000000);
	pwmSetPolarity(pwm, 0);



	return 0;
}
