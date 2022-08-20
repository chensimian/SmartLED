package com.sq.wolf;

import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.Handler;
import android.os.IBinder;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.Toast;

import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.util.Timer;
import java.util.TimerTask;

public class MainActivity extends AppCompatActivity {
    private final static String TAG = "MainActivity";
    private LinearLayout ll_locate;
    private NavigationCustom nc_model;
    private NavigationCustom nc_adc;
    private NavigationCustom nc_pwm;
    private NavigationCustom nc_hcr;
    private SwitchCustom sc_led;
    private ImageView iv_led;
    private SeekBar sb_pwm;
    private ACache cache;
    private MqttService.MqttBinder mqttBinder;
    private CommReceiver commReceiver;
    private String imei;
    private String ip;
    private String port;
    private boolean led = false;
    private int model = 1;
    private int set_pwm = 0;
    private Handler handler = new Handler();
    private Timer timer;
    class CommReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            switch (intent.getIntExtra("msg", 0)) {
                case MqttService.CONNECTED:
                    Toast.makeText(MainActivity.this, "连上了服务器", Toast.LENGTH_LONG).show();
                    break;
                case MqttService.FAIL:
                    Toast.makeText(MainActivity.this, "连接失败，5S后重连", Toast.LENGTH_SHORT).show();
                    handler.postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            mqttBinder.connect("tcp://" + ip + ":" + port,"admin", "password", imei);
                        }
                    }, 5000);
                    break;
                case MqttService.DISCONNECTED:
                    Toast.makeText(MainActivity.this, "连接断开，5S后重连", Toast.LENGTH_SHORT).show();
                    handler.postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            mqttBinder.connect("tcp://" + ip + ":" + port, "admin", "password", imei);
                        }
                    }, 5000);
                    break;
                case MqttService.RECV:
                    String content = intent.getStringExtra("content");
                    Log.e(TAG, content);
                    updateUI(content);
                    break;
                default:
                    break;

            }
        }
    }

    private ServiceConnection servConn = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            mqttBinder = (MqttService.MqttBinder)service;
            mqttBinder.connect("tcp://" + ip + ":" + port, "admin", "password",imei);
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {

        }
    };
  ///接收展示
    private void updateUI(String str) {
        char [] strArr1 = str.toCharArray();
        if(strArr1[0]=='U')
        {
            if (timer != null) {
                timer.cancel();
                timer = null;

                timer = new Timer();
                timer.schedule(new TimerTask() {
                    @Override
                    public void run() {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                if (mqttBinder.getConnStatus()) {
                                    Toast.makeText(MainActivity.this, "设备下线了", Toast.LENGTH_LONG).show();
                                }
                            }
                        });
                    }
                }, 60000);
            }
            model = strArr1[1]-0x30;
            int led = strArr1[2]-0x30;
            int hc = strArr1[3]-0x30;
            int adc = (strArr1[4]-0x30)*100+(strArr1[5]-0x30)*10+(strArr1[6]-0x30);
            int pwm = (strArr1[7]-0x30)*100+(strArr1[8]-0x30)*10+(strArr1[9]-0x30);

            if(model==0)
            {
                nc_model.setRightText("手动模式");
                sc_led.setStatus(false);
            }
            else
            {
                nc_model.setRightText("自动模式");
                sc_led.setStatus(true);
            }
            if(hc==0)
            {
                nc_hcr.setRightText("无人");
            }
            else
            {
                nc_hcr.setRightText("有人");
            }

            nc_adc.setRightText(Integer.toString(adc));

            nc_pwm.setRightText(Integer.toString(pwm));
            set_pwm = pwm;
            sb_pwm.setProgress(pwm);
            sb_pwm.setEnabled(true);
            iv_led.setImageResource(led == 0 ?  R.drawable.light_off : R.drawable.light_on);


        }
    }

    private void initPara() {
        cache = ACache.get(this);
        imei = cache.getAsString("imei");
        ip = cache.getAsString("ip");
        port = cache.getAsString("port");
        commReceiver = new CommReceiver();
        Intent intent = new Intent(MainActivity.this, MqttService.class);
        bindService(intent, servConn, BIND_AUTO_CREATE);
        IntentFilter filter = new IntentFilter("com.sq.wolf.mqtt");
        registerReceiver(commReceiver, filter);
    }

    private void initUI() {

        nc_model = (NavigationCustom)findViewById(R.id.nc_model);
        nc_adc = (NavigationCustom)findViewById(R.id.nc_hj);
        nc_hcr = (NavigationCustom)findViewById(R.id.nc_hc);
        nc_pwm = (NavigationCustom)findViewById(R.id.nc_pwm);
        sc_led = (SwitchCustom)findViewById(R.id.sc_led);
        iv_led = (ImageView)findViewById(R.id.iv_led);
        sb_pwm = (SeekBar)findViewById(R.id.seekBar);
        sb_pwm.setMax(999);
        sb_pwm.setEnabled(false);
    }

    private void initSeekBar() {
        sb_pwm.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {


            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

                    MqttMessage msg = new MqttMessage();
                    msg.setQos(0);
                    msg.setRetained(false);
                    set_pwm = seekBar.getProgress();
                    String pwm = String.format("%0" + 3 + "d", set_pwm);
                    msg.setPayload(("S"+model+pwm).getBytes());
                    mqttBinder.publish("sldev/" + imei, msg);
                }

        });
    }


    private void initSwitchCustom() {
        sc_led.setOnSwitchCustomSelectedListener(new SwitchCustom.OnSwitchCustomSelectedListener() {
            @Override
            public void onSelected(boolean isChecked) {
                if (!mqttBinder.getConnStatus()) {
                    sc_led.setStatus(isChecked ? false : true);
                    Toast.makeText(MainActivity.this, "未连上服务器", Toast.LENGTH_SHORT).show();
                    return;
                }
                led = isChecked;
                MqttMessage msg = new MqttMessage();
                msg.setQos(0);
                msg.setRetained(false);
                if(led == true)
                {
                    model = 1;
                }
                else
                {
                    model = 0;
                }
                //发送出去的
                String pwm = String.format("%0" + 3 + "d", set_pwm);
                msg.setPayload(("S"+model+pwm).getBytes());//S1001~S1115
                mqttBinder.publish("sldev/" + imei, msg);
            }
        });
    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initPara();
        initUI();

        initSwitchCustom();
        initSeekBar();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mqttBinder.disconnect();
        unbindService(servConn);
        unregisterReceiver(commReceiver);
    }
}
