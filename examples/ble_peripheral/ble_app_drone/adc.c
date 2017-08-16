#include "adc.h"

volatile uint8_t state = 1;

const nrf_drv_timer_t m_timer = NRF_DRV_TIMER_INSTANCE(1);
nrf_saadc_value_t     m_buffer_pool[1][SAMPLES_IN_BUFFER];
nrf_ppi_channel_t     m_ppi_channel;
uint32_t              m_adc_evt_counter;
nrf_saadc_value_t m_adc_value[2];
uint32_t millis;
void timer_handler(nrf_timer_event_t event_type, void * p_context)
{
}


void saadc_sampling_event_init(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_ppi_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32;
    err_code = nrf_drv_timer_init(&m_timer, &timer_cfg, timer_handler);
    APP_ERROR_CHECK(err_code);

    /* setup m_timer for compare event every 10ms */
    uint32_t ticks = nrf_drv_timer_ms_to_ticks(&m_timer, 1);
    nrf_drv_timer_extended_compare(&m_timer,
                                   NRF_TIMER_CC_CHANNEL0,
                                   ticks,
                                   NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK,
                                   false);
    nrf_drv_timer_enable(&m_timer);

    uint32_t timer_compare_event_addr = nrf_drv_timer_compare_event_address_get(&m_timer,
                                                                                NRF_TIMER_CC_CHANNEL0);
    uint32_t saadc_sample_task_addr   = nrf_drv_saadc_sample_task_get();

    /* setup ppi channel so that timer compare event is triggering sample task in SAADC */
    err_code = nrf_drv_ppi_channel_alloc(&m_ppi_channel);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_ppi_channel_assign(m_ppi_channel,
                                          timer_compare_event_addr,
                                          saadc_sample_task_addr);
    APP_ERROR_CHECK(err_code);
}


void saadc_sampling_event_enable(void)
{
    ret_code_t err_code = nrf_drv_ppi_channel_enable(m_ppi_channel);

    APP_ERROR_CHECK(err_code);
}

	uint16_t rate;
void saadc_callback(nrf_drv_saadc_evt_t const * p_event)
{

    if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
    {
        ret_code_t err_code;

        err_code = nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, SAMPLES_IN_BUFFER);
        APP_ERROR_CHECK(err_code);
				
				millis++;
				getValue(m_adc_value[1]);
				rate = digitalGetRate(millis);
//				printf("%d \n",millis);
//				printf("%d \n", getValue(m_adc_value[1]));
				if(rate)
				{
					printf("%d \n",rate);
				}
        for (int i = 0; i < SAMPLES_IN_BUFFER; i++)
        {
						m_adc_value[i] = p_event->data.done.p_buffer[i];
						//printf("%d  %d\r\n", m_adc_value[0], m_adc_value[1]);
						if(m_adc_value[i] < 0)
						{
							m_adc_value[i] = 0;
						}
        }
    }
}

void saadc_init(void)
{
    ret_code_t err_code;
    nrf_saadc_channel_config_t channel_config =
        NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN0);
	
		nrf_saadc_channel_config_t channel_config2 =
			NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN1);
			channel_config2.gain = NRF_SAADC_GAIN1_5;
    err_code = nrf_drv_saadc_init(NULL, saadc_callback);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_channel_init(0, &channel_config);
    APP_ERROR_CHECK(err_code);
	
		err_code = nrf_drv_saadc_channel_init(1, &channel_config2);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[0], SAMPLES_IN_BUFFER);
    APP_ERROR_CHECK(err_code);
}
#define ANALOG_MODE 0
#define DIGITAL_MODE 1


#define SAMPLE_NUMBER 100
uint8_t valueCount_=255;
uint16_t value[SAMPLE_NUMBER]={0}; ///< Initialize sampling point value
uint8_t mode_;
uint8_t valueFlag;	
uint32_t nowTim=0,lastTim=0;


uint16_t getValue(nrf_saadc_value_t adc_value) 
{
	valueCount_++;
    if(valueCount_ >= SAMPLE_NUMBER){
        valueCount_ = 0;
    }    
	value[valueCount_] = adc_value;  
    return(value[valueCount_]);
}


/*!
* @brief Points to a valid node of the wave rise
*
* @brief  When the sampling point value increased 4 times in a row, considered to be a heartbeat
*
* @return char
*/ 

char maxNumber(uint8_t count)
{
	uint16_t temp1,temp2;
	for(int i=0;i<4;i++){
		if(count<i){
			temp1 = SAMPLE_NUMBER+count-i;
		}else{
			temp1 = count-i;
		}
		if(count<i+1){
			temp2 = SAMPLE_NUMBER+(count-1)-i;
		}else{
			temp2 = (count-1)-i;
		}		
		if(value[temp1]<=value[temp2])return(0);
	}
	if(valueFlag){
		valueFlag=1;
		return(0);
	}else{
		valueFlag=1;
		return(1);
	}	
}

/*!
* @brief Waveform down processing
*
* @brief  When the sample values decreased four times in a row, clear the heartbeat
*
* @return void
*/ 

void minNumber(uint8_t count)
{
	uint16_t temp1,temp2;
	for(int i=0;i<4;i++){
		if(count<i){
			temp1 = SAMPLE_NUMBER+count-i;
		}else{
			temp1 = count-i;
		}
		if(count<i+1){
			temp2 = SAMPLE_NUMBER+(count-1)-i;
		}else{
			temp2 = (count-1)-i;
		}		
		if(value[temp1]>=value[temp2])return;
	}
	valueFlag = 0;
	
}


/*!
* @brief Get heart rate value
*
* @brief  Sample ten times in a row beating heart rate value is computed
*
* @return uint16_t
*/ 

uint8_t digitalGetRate(uint32_t millis)
{

	static uint8_t timeFlag;
	static unsigned long sampleTime[10];
	unsigned long valueTime_;
	uint8_t count_;
	
	if(valueCount_){
		count_ = valueCount_-1;
	}else{
		count_ = SAMPLE_NUMBER-1;
	}
	if((value[valueCount_]>1000)&&(value[count_]<20)){       
        nowTim = millis;
		uint32_t difTime =  nowTim - lastTim;
        lastTim = nowTim;
        
        if(difTime>300 || difTime<2000){
            sampleTime[timeFlag++] = difTime;
            if(timeFlag > 9)timeFlag=0;
        }       
        if(0 == sampleTime[9]){
            printf("1 Wait for valid data !\n"); 
            return(0);
        }
        		
		uint32_t Arrange[10]={0};		
		for(int i=0;i<10;i++){
            Arrange[i] = sampleTime[i];
        }
        uint32_t Arrange_=0;
        for(int i=9;i>0;i--){				// sorting
            for(int j=0;j<i;j++){
                if(Arrange[j] > Arrange[j+1]){
                Arrange_ = Arrange[j];
                Arrange[j] = Arrange[j+1];
                Arrange[j+1] = Arrange_;
                }
            }            
        }   
		if((Arrange[7]-Arrange[3])>120){
            printf("2 Wait for valid data !\n");  
			return(0);
		}	
        
        Arrange_ = 0;
		for(int i=3;i<=7;i++){		// reduce too short time or too long time
			Arrange_ += Arrange[i];
		}
		
		valueTime_ = 300000/Arrange_;///<60*1000*(7-2)	
		return((uint16_t)valueTime_);
    }	
	return(0);

}

