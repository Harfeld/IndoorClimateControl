#ifndef MQ_135_H_
#define MQ_135_H_

enum Gas{
	CO2 = 0
};

void Init_MQ();
void MqCalibrate(enum Gas gas);
float MQGetGasPercentage(enum Gas gas);

#endif /* MQ_135_H_ */