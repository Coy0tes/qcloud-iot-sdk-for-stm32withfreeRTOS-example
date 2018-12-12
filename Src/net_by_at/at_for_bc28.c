/*********************************************************************************
 *                   Copyright (c) 2018 - 2020,Tencent
 *                      All rights reserved.
 *
 * File Name:    at_for_bc28.c
 *
 * Description:   net api based on at cmd for l206
 *
 * History:      <author>          <time>        		<version>
 *                   yougaliu          2018-11-26             1.0
 * Desc:           ORG.
 ********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "at_driver.h"
#include "at_for_bc28.h"
#include "string.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

#include "time.h"

/* External variables --------------------------------------------------------*/

void HexDump(uint8_t *pdata, int len)
{
	for(int i=0; i < len; i++)
	{
		if(0 == i%16)
		{
			printf("\n\r");
		}
		printf("%02X ", pdata[i]);
	}
}



/**
  * @brief �رջ���
  */

HAL_StatusTypeDef atEchoOff(void)
{
	sAtCmd atCmd;	

	//�رջ���	
	init_send_cmd(&atCmd,  "ATE0\r\n", "OK", TIMOUT_3S);
	
	return exec_gsm_at(&atCmd);
}

/**
  * @brief �򿪻���
  */

HAL_StatusTypeDef atEchoOn(void)
{
	sAtCmd atCmd;	

	//�򿪻���	
	init_send_cmd(&atCmd,  "ATE1\r\n", "OK", TIMOUT_1S);
	
	return exec_gsm_at(&atCmd);
}

/**
  * @brief AT ״̬ȷ��
  */

HAL_StatusTypeDef atAlive(void)
{
	sAtCmd atCmd;	

	init_send_cmd(&atCmd,  "AT\r\n", "OK", TIMOUT_1S);
	
	return exec_gsm_at(&atCmd);
}


HAL_StatusTypeDef atReset(void)
{
	sAtCmd atCmd;	

	init_send_cmd(&atCmd,  "AT+NRB\r\n", NULL, TIMOUT_1S);
	
	return exec_gsm_at(&atCmd);
}



int net_init_by_at(void)
{
	sAtCmd atCmd;	
	int Ret = HAL_ERROR;
	int count;

	HwInitBc28();
	
	if(HAL_OK != atAlive())
	{
		AT_INFO_DEBUG("\n\rBC28 not ready");
		//goto end;
	}

	
	if(HAL_OK !=  atEchoOff())
	{
		AT_INFO_DEBUG("\n\ratEcho off fail");
	}

#if 0 //just for test
	init_send_cmd(&atCmd, "ATI\r\n", "OK", TIMOUT_1S);
	if(HAL_OK !=  exec_gsm_at(&atCmd))
	{
		AT_INFO_DEBUG("\n\rATI fail");
		//goto end;
	}

	init_send_cmd(&atCmd, "AT+CGMR\r\n", "OK", TIMOUT_1S);
	if(HAL_OK !=  exec_gsm_at(&atCmd))
	{
		AT_INFO_DEBUG("\n\rCGMR fail");
		//goto end;
	}

	init_send_cmd(&atCmd, "AT+QDNS=?\r\n", "OK", TIMOUT_1S);
	if(HAL_OK !=  exec_gsm_at(&atCmd))
	{
		AT_INFO_DEBUG("\n\rQDNS fail");
		//goto end;
	}
#endif	

	init_send_cmd(&atCmd, "AT+CFUN?\r\n", "+CFUN:1", TIMOUT_1S);
	for(count = 0; count < (AT_CHECK_STAT_TIME + 1); count++)
	{		
		if(HAL_OK ==  exec_gsm_at(&atCmd))
		{
			break;
		}
		else
		{
			if(AT_CHECK_STAT_TIME == count)
			{
				AT_INFO_DEBUG("\n\rNot full  functionality");
				goto end;
			}
		}
	}


	init_send_cmd(&atCmd, "AT+CSQ\r\n",  NULL, TIMOUT_1S);
	if(HAL_OK !=  exec_gsm_at(&atCmd))
	{
		goto end;
	}
	else
	{		
		AT_INFO_DEBUG("\n\rGet CSQ:[%s]", atCmd.pAckBuf);
		if((strstr(atCmd.pAckBuf,"+CSQ:99,99")) || (strstr(atCmd.pAckBuf,"+CSQ:00,99")))
		{
			AT_INFO_DEBUG("\n\rCSQ ERROR");
			//goto end;
		}
	}


	/*ʹ���ֶ���ȡ����*/
	init_send_cmd(&atCmd, "AT+CIMI\r\n", NULL, TIMOUT_1S);
	if(HAL_OK !=  exec_gsm_at(&atCmd))
	{
		goto end;
	}
	else
	{
		AT_INFO_DEBUG("\n\rGet IMI:[%s]", atCmd.pAckBuf);
	}

	
	init_send_cmd(&atCmd, "AT+CGATT?\r\n", "+CGATT:1", TIMOUT_2S);

	for(count = 0; count < (AT_CHECK_STAT_TIME + 1); count++)
	{
		if(HAL_OK ==  exec_gsm_at(&atCmd))
		{
			break;
		}
		else
		{
			if(AT_CHECK_STAT_TIME == count)
			{
				goto end;
			}
		}
	}
	
	init_send_cmd(&atCmd, "AT+CPSMS=2\r\n", "OK", TIMOUT_1S);
	if(HAL_OK !=  exec_gsm_at(&atCmd))
	{
		goto end;
	}

	Ret = HAL_OK;

end:

	return Ret;	
}


int net_get_fd(eNetProto protocol, int port)
{
	int fd;
	sAtCmd atCmd;	
	char CmdStr[IP_CMD_LEN];
	char fdstr[5];

	memset(CmdStr, 0, IP_CMD_LEN);
	if(eUDP == protocol)
	{					
		snprintf(CmdStr, IP_CMD_LEN, "AT+NSOCR=DGRAM,%d,%d,1\r\n", protocol, port);	
	}	
	else
	{
		snprintf(CmdStr, IP_CMD_LEN, "AT+NSOCR=STREAM,%d,%d,1\r\n", protocol, port);
	}
	AT_INFO_DEBUG("\n\rProtocol[%s]", CmdStr);	
	init_send_cmd(&atCmd, CmdStr, "OK", TIMOUT_5S);

	if(HAL_OK ==  exec_gsm_at(&atCmd))
	{				
		memset(fdstr, 0, 5);
		strncpy(fdstr, ((char *)atCmd.pAckBuf+2), 2); //need to be optimized
		if((0x0D == fdstr[1])||(0x0A == fdstr[1]))
		{
			fdstr[1] = '\0';
		}
		else
		{
			fdstr[2] = '\0';
		}
		fd = atoi(fdstr);
		AT_INFO_DEBUG("\n\rfd[%d] fdStr[%s]", fd, atCmd.pAckBuf);
		HexDump(atCmd.pAckBuf, strlen(atCmd.pAckBuf));
	}	
			
	return fd;
}

int net_dns(const char *host, char *ip)
{
	int fd;
	sAtCmd atCmd;	
	char CmdStr[IP_CMD_LEN];


	AT_INFO_DEBUG("\n\r[net_dns] host[%s]", host);
	memset(CmdStr, 0, IP_CMD_LEN);
	snprintf(CmdStr, IP_CMD_LEN, "AT+QDNS=0,\"%s\"\r\n", host);	
	init_send_cmd(&atCmd, CmdStr, "OK", TIMOUT_5S);
	if(HAL_OK ==  exec_gsm_at(&atCmd))
	{
		AT_INFO_DEBUG("\n\ripStr[%s]", atCmd.pAckBuf);		
	}
	
	strncpy(ip,"123.207.117.108", IP_STR_LEN);  //test server
	return 0;
}


int net_connect_by_at(const char *host,  int port, int *pfd,  eNetProto protocol, void* pdata)
{
	sAtCmd atCmd;	
	char CmdStr[IP_CMD_LEN];
	char IpAddr[IP_STR_LEN];
	int Ret  = HAL_ERROR;

	/*BC need do dns first*/
	memset(IpAddr, 0, IP_STR_LEN);
	net_dns(host, IpAddr);

	/*Get fd*/	
	*pfd = net_get_fd(protocol, port);


	/*set up connect*/	
	memset(CmdStr, 0, IP_CMD_LEN);
	snprintf(CmdStr, IP_CMD_LEN, "AT+NSOCO=%d,%s,%d\r\n", *pfd, IpAddr, port);
	AT_INFO_DEBUG("\n\ripCmdStr[%s]", CmdStr);
	
	init_send_cmd(&atCmd, CmdStr, "OK", 2*TIMOUT_5S);
	
	AT_INFO_DEBUG("\n\rat_buf[%s]", atCmd.at_buf);
	
	if(HAL_OK ==  exec_gsm_at(&atCmd))
	{
		Ret = HAL_OK;
		AT_INFO_DEBUG("\n\rnet connect setup success");
	}
	else
	{
		Ret = HAL_ERROR;
		AT_INFO_DEBUG("\n\rnet connect setup fail");
	}

	return Ret;	
}

int net_disconnect_by_at(uint32_t fd, void* pdata)
{
	sAtCmd atCmd;	
	char CmdStr[IP_CMD_LEN];
	int Ret = HAL_ERROR;

	memset(CmdStr, 0, IP_CMD_LEN);
	snprintf(CmdStr, IP_CMD_LEN, "AT+NSOCL=%d\r\n", fd);

	
	init_send_cmd(&atCmd, CmdStr, "OK", TIMOUT_1S);
	if(HAL_OK ==  exec_gsm_at(&atCmd))
	{
		Ret = HAL_OK;
		AT_INFO_DEBUG("\n\rnet disconnect success");
	}
	else
	{
		Ret = HAL_ERROR;
		AT_INFO_DEBUG("\n\rnet disconnect fail");
	}
	
	return Ret;	
}


int net_connectState_by_at(uint32_t fd, void* pdata)
{

	return HAL_OK;	
}

int net_destroy_by_at(uint32_t fd,void* pdata)
{
	return HAL_OK;	
}


/*
*�·�AT+CIPRXGET=2,len�ֶ���ȡģ��AT buf�е�����
* ackbuf�л�õ���ʽ���µ�����
   +CIPRXGET: 2,len,rest_len \n\r
   xxxxxx\n\r   (��������)
   OK
**������������ʽ��atbuf����ȡ��ָ����������
*/

static int copyDataToBuf(uint8_t *pFromBuf, uint8_t *pToBuf, uint16_t len)
{
	uint8_t *pPosition = NULL;
	uint8_t lenChar[RECV_LEN_NUM_MAX + 1] = {'\0'};
	int i = 0;
	int num;
	int readlen;
	int	offset = 0;
	bool startflag = false;
	int Ret = RET_ERR;
	int delayTime;

	if(!pFromBuf || !pToBuf || (len < 1))
	{
		return RET_ERR;
	}

	
	//HAL_Delay(500);

	delayTime = (len/100)*100 + 350;
	HAL_Delay(delayTime);

	pPosition = strstr(pFromBuf, READ_CMD_ACK_PATTEN);
	if(NULL != pPosition)
	{
		offset = strlen(READ_CMD_ACK_PATTEN);
		
		for(num = 0; num < RECV_LEN_NUM_MAX; num++)
		{
			if(NUM_END_PATTEN != pPosition[offset + num])
			{
				lenChar[num] = pPosition[offset + num];	
			}
			else
			{
				readlen = atoi(lenChar);
				len = (readlen > NET_READ_MAXLEN_ONCE)?len:readlen;				
			}							
		}
		
		
		for(i = 0; i < (AT_RECIVE_MAX_BUF_LEN - offset); i++)
		{
			if((END_PATTEN1 == pPosition[offset + i])||(END_PATTEN2 == pPosition[offset + i]))
			{
				if((END_PATTEN1 != pPosition[offset + i + 1])&&(END_PATTEN2 != pPosition[offset + i + 1]))
				{	
					startflag = true;
					break;
				}
			}		
		}

		if(true == startflag)
		{
			pPosition += offset + i + 1; 
			memcpy(pToBuf, pPosition, len);
			Ret = len;			
		}
	}

	return Ret;
}

int net_read_by_at(uint32_t fd, uint8_t *pRecvbuf, int len, uint16_t timeOut)
{
	sAtCmd atCmd;		
	char readCmdStr[IP_CMD_LEN];
	uint8_t count = 0;
	int readLen;
	int retlen = 0;
	int Ret = HAL_ERROR;
	uint32_t ulStartTick;
	uint32_t ulEndTick;	

	HAL_Delay(300);

	ulStartTick = HAL_GetTick();
	ulEndTick = ulStartTick + timeOut;

	while((len > 0)&&(ulEndTick > HAL_GetTick()))
	{		
		if(len > NET_READ_MAXLEN_ONCE)
		{
			readLen = NET_READ_MAXLEN_ONCE;	
			len -= NET_READ_MAXLEN_ONCE;
			count++;
		}
		else
		{
			readLen = len;
			len = 0;
		}

		memset(readCmdStr, 0, IP_CMD_LEN);
		snprintf(readCmdStr, IP_CMD_LEN, "AT+NSORF=%d,%d\r\n", fd, readLen);	
		init_send_cmd(&atCmd, readCmdStr, READ_CMD_ACK_PATTEN, TIMOUT_5S);
		
		if(HAL_OK ==  exec_gsm_at(&atCmd))
		{
			retlen = copyDataToBuf((uint8_t *)atCmd.pAckBuf, (pRecvbuf + count*NET_READ_MAXLEN_ONCE) , readLen);
			if(RET_ERR ==  retlen)
			{			
				AT_INFO_DEBUG("\n\r[net_read_by_at]copyDataToBuf err");	
				Ret = HAL_ERROR;
				break;
			}
			else if(retlen < readLen)
			{
				AT_INFO_DEBUG("\n\r[net_read_by_at]copyDataToBuf data less than read needed");	
				Ret = count*NET_READ_MAXLEN_ONCE + retlen;
				break;
				
			}else if(0 == len)
			{
				Ret = count*NET_READ_MAXLEN_ONCE + retlen;
			}
		}
		else
		{
			Ret = HAL_ERROR;
			break;
		}		
	}	

	AT_INFO_DEBUG("\n\r[net_read_by_at] return [%d]", Ret);	

	return Ret;	
}


int net_write_by_at(uint32_t fd, uint8_t *pSendbuf, int len, uint16_t timeOut)
{
	sAtCmd atCmd;		
	char writeCmdStr[IP_CMD_LEN];
	uint8_t count = 0;
	int sendLen;
	int Ret = HAL_ERROR;
	uint32_t ulStartTick;
	uint32_t ulEndTick;	

	ulStartTick = HAL_GetTick();
	ulEndTick = ulStartTick + timeOut;

	while((len > 0)&&(ulEndTick > HAL_GetTick()))
	{		
		if(len > NET_SEND_MAXLEN_ONCE)
		{
			sendLen = NET_SEND_MAXLEN_ONCE;	
			len -= NET_SEND_MAXLEN_ONCE;
			count++;
		}
		else
		{
			sendLen = len;
			len = 0;
		}

		memset(writeCmdStr, 0, IP_CMD_LEN);
		snprintf(writeCmdStr, IP_CMD_LEN, "AT+NSOSD=%d,%d,%s\r\n", fd, sendLen, pSendbuf);			
		init_send_cmd(&atCmd, writeCmdStr, "OK", TIMOUT_5S);
		
		if(HAL_OK ==  exec_gsm_at(&atCmd))
		{
			Ret = HAL_OK;
#if 0		
			if(HAL_ERROR == (at_send_data(pSendbuf + count*NET_SEND_MAXLEN_ONCE, sendLen)))
			{
				AT_INFO_DEBUG("\n\r[net_write_by_at]at_send_data err");	
				Ret = HAL_ERROR;
				
			}else if(0 == len)
			{
				Ret = HAL_OK;
			}
#endif			
		}
		else
		{
			Ret = HAL_ERROR;
			break;
		}
	}

//	if(HAL_OK == WaitForAck("SEND OK",  TIMOUT_5S))
//	{
//		if(HAL_OK == Ret)
//		{
//			Ret = count*NET_SEND_MAXLEN_ONCE + sendLen;
//		}
//	}	
//	else
//	{
//		AT_INFO_DEBUG("\n\r[WaitForAck]timeout");		
//	}

	return Ret;	
}



/*
*��ȡlocaltime����̬token��ȡ��Ҫʱ���
*�ȶ� http://gz.auth-device-iot.tencentcloudapi.com/time
*/
int GetLocalTimeByAt(uint8_t * strTime)
{
	sAtCmd atCmd;	 
	int Ret = HAL_ERROR;
	
	
	init_send_cmd(&atCmd, "AT+CCLK?\r\n", "OK", TIMOUT_2S);
	if(HAL_OK ==  exec_gsm_at(&atCmd))
	{
		Ret = HAL_OK;	
		sscanf((uint8_t *)atCmd.pAckBuf, "%*s %s %*s", strTime);  
		AT_INFO_DEBUG("\n\rGetLocalTime [%s]", atCmd.pAckBuf);
	}
	else
	{
		Ret = HAL_ERROR;
		AT_INFO_DEBUG("\n\rGetLocalTimeByGsm fail");
	}
	
	return Ret; 
}


/*
* strTime: string time input,  "18/06/19,17:35:55+32"
* ������ʱ��ת��Ϊ���UTC ����ʱ��
*/
long GetTimeStampByAt(void *zone)
{
	char strTime[TIME_STR_LEN];
	char YYtimeChar[TIME_STR_LEN];
	char HHtimeChar[TIME_STR_LEN];
	uint8_t *pHH = NULL;
	int yy, mm, dd, hh, mimi, ss;
	struct tm tm_time;
	long unixtime;
	int zoneTime;

	
	memset(strTime, 0, TIME_STR_LEN);
	memset(YYtimeChar, 0, TIME_STR_LEN);
	memset(HHtimeChar, 0, TIME_STR_LEN);

	if(HAL_ERROR == GetLocalTimeByAt(strTime))
	{
		unixtime = 0;
		goto end;
	}
	
	
	sscanf(strTime + 1, "%[^,]", YYtimeChar);  	
	pHH = strchr(strTime, ',');
	sscanf(pHH + 1, "%[^+]%", HHtimeChar); 

	
	sscanf(YYtimeChar, "%d/%d/%d", &yy, &mm, &dd);	
	sscanf(HHtimeChar, "%d:%d:%d", &hh, &mimi, &ss);

	tm_time.tm_year = yy + TIME_BASE_YEAR - 1900;
	tm_time.tm_mon = mm - 1;
	tm_time.tm_mday = dd;
	tm_time.tm_hour = hh;
	tm_time.tm_min = mimi;
	tm_time.tm_sec = ss;

	if(NULL == zone)
	{
		zoneTime = ZONE_FOR_BEIJING;
	}
	
	unixtime = mktime(&tm_time);
	unixtime -= zoneTime*3600;   
	
	AT_INFO_DEBUG("\n\rYYtime[%s] HHtime[%s]",YYtimeChar, HHtimeChar);
	AT_INFO_DEBUG("\n\r[NewTime]%d-%d-%d %d:%d:%d", yy + TIME_BASE_YEAR, mm, dd, hh, mimi, ss);
	AT_INFO_DEBUG("\n\rUTC [%d]s", unixtime);


end:

	return unixtime;
}


/**
* @ �ϵ�ʱ��Ҫ��μ�gsm ģ��Ҫ��
*/
void HwInitBc28(void)
{
	int count;

	//atReset();
	//atReset();
	//HAL_Delay(5000);
	
	for(count = 0; count < AT_REPEAT_TIME; count++)
	{
		if(HAL_OK == atAlive())
		{	
			AT_INFO_DEBUG("\n\rAT Ready");
			break;
		}
	}

#if (BOARD_TYPE == BOARD_TYPE_SENSOR)
	if(count > (AT_REPEAT_TIME - 1))
	{	
		/*AT not ready,power off and on*/
		AT_INFO_DEBUG("\n\rAT not Ready"); 		
		atReset();
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		HAL_Delay(POWER_OFF_TIME);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	}
#else
	#error unknow board type
#endif
}

/***********************END OF FILE**************************/
