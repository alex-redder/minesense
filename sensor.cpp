#include <iostream>
//#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>   
#include <windows.h>
#include <time.h>
 

using namespace std;

int data[] = {100, 101, 105, 110, 210, 100, 102, 110, 150, 100};

class CSensor
{
public:
	bool Calibrate() // calibrates the sensor
	{ 
		cout<<"Calibrating......"<<endl;
		Sleep(1000);
		cout<<"Calibrated"<<endl;
		return true;
	}

	bool ReadData(int data[]) // Reads the data from the sensor
	{ 
		int summary = 0;
		for(int i = 0; i<sizeof(::data)/sizeof(::data[0]); i++)
		{
			summary += data[i];
		}

		if(i>0)
		{
			average = summary/i;
			cout<<"Read average value: "<<average<<endl;
		}
		else
		{
			average = 0;
			return false;
		}

		return true;
	}

	int getPosition()
	{
		int pos = average;
		return pos;
	}

private:
	int average;
};

class CMotor
{
public:
	CMotor(int sPosition) : startingPosition(sPosition), currentPosition(sPosition)
	{
	}

	void ResetMotor() // moves motor to the starting position
	{ 
		MoveMotor(startingPosition - currentPosition);
		currentPosition = startingPosition;
	}

	/* Moves motor to the measurement position, 
		supposed the Motor in one dimension, 
		distance>0 is forward, 
		distance<0 is backward 
	*/
	bool MoveMotor(int distance) 
	{ 
		cout<<"move: "<<distance<<endl;
		return true;
	}

	bool move(int position)
	{	
		bool ret;
		ret = MoveMotor(position - currentPosition);
		currentPosition = position;
		return ret;
	}

private:
	int startingPosition;
	int currentPosition;
};

class CDoorLatch
{
public:
	bool isOpen()
	{
		return open;
	}
	
	void getLatch()
	{
		srand((unsigned)time(0));
		int doorPos = rand()%10;
		if (doorPos>7) 
		{
			open=true;
		}
		else
		{
			open = false;
		}
		cout<<"open: "<<open<<", door pos:"<<doorPos<<endl;
	}

private:
	bool open;
};

typedef enum{
	INITIAL = 1,
	CALIBRATED,
	READ,
	MOVED,
	DISABLED,
	FAULT
}STATE_MACHINE;


int main()
{
	STATE_MACHINE stateMachine = DISABLED;
	CMotor motor(0);
	CSensor sensor;
	CDoorLatch doorLatch;

	while(1)
	{
		doorLatch.getLatch();
		if( !doorLatch.isOpen() )
		{
			if(DISABLED == stateMachine)
			{
				stateMachine = INITIAL;
				motor.ResetMotor();
			}

			if(INITIAL == stateMachine)
			{	
				if (sensor.Calibrate()) 
				{
					stateMachine=CALIBRATED;
				}
				else
				{
					stateMachine=FAULT;
					cout<<"ALARM: Calibrate error!"<<endl;
				}
			}
			
			if(CALIBRATED == stateMachine || MOVED == stateMachine)
			{
				if(sensor.ReadData(data))
				{
					stateMachine=READ;
				}
				else
				{
					stateMachine=FAULT;
					cout<<"ALARM: Read error!"<<endl;
				}

			}

			if(READ == stateMachine)
			{
				int pos = sensor.getPosition();

				if(motor.move(pos))
				{
					stateMachine = MOVED;
				}
				else
				{
					stateMachine=FAULT;
					cout<<"ALARM: Move Motor error!"<<endl;
				}

			}

			
		}
		else
		{
			stateMachine=DISABLED;
			cout<<"ALARM:door is open!"<<endl;
			Sleep(5000);
		}

		Sleep(2000);
		cout<<endl;
	}

	return 0;
}
