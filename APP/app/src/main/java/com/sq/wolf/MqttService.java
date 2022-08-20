package com.sq.wolf;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.support.annotation.Nullable;
import android.util.Log;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

/**
 * Created by chensimian.
 */

public class MqttService extends Service {
    private final static String TAG = "MqttService";
    public final static int CONNECTED = 1;
    public final static int FAIL =2;
    public final static int DISCONNECTED = 3;
    public final static int RECV = 4;
    private MqttBinder binder = new MqttBinder();
    private MemoryPersistence persistence = null;
    private MqttClient mqttClient = null;
    private MqttConnectOptions mqttConnOpts = null;
    private boolean connFlag = false;

    public class MqttBinder extends Binder {
        public void connect(String url, String user, String pass, String clientId) {
            Log.e(TAG, "connect");
            persistence = new MemoryPersistence();
            try {
                mqttClient = new MqttClient(url, "slapp/" + clientId, persistence);
            }catch (MqttException e) {
                e.printStackTrace();
                Intent intent = new Intent("com.sq.wolf.mqtt");
                intent.putExtra("msg", FAIL);
                sendBroadcast(intent);
            }

                mqttConnOpts = new MqttConnectOptions();
                mqttConnOpts.setCleanSession(true);
                mqttConnOpts.setConnectionTimeout(10);
                mqttConnOpts.setKeepAliveInterval(20);
                mqttConnOpts.setUserName(user);
                mqttConnOpts.setPassword(pass.toCharArray());


                mqttClient.setCallback(new MqttCallback() {
                    @Override
                    public void connectionLost(Throwable throwable) {
                        connFlag = false;
                        Intent intent = new Intent("com.sq.wolf.mqtt");
                        intent.putExtra("msg", DISCONNECTED);
                        sendBroadcast(intent);
                    }

                    @Override
                    public void messageArrived(String s, MqttMessage mqttMessage) throws Exception {
                        Log.e(TAG, s);
                        String str = new String(mqttMessage.getPayload());
                        if (str != null && !str.equals("")) {
                            Intent intent = new Intent("com.sq.wolf.mqtt");
                            intent.putExtra("msg", RECV);
                            intent.putExtra("content", str);
                            sendBroadcast(intent);
                        }
                    }

                    @Override
                    public void deliveryComplete(IMqttDeliveryToken iMqttDeliveryToken) {

                    }
                });
            try {
                mqttClient.connect(mqttConnOpts);
                Log.e(TAG, "slapp/" + clientId);
                mqttClient.subscribe("slapp/" + clientId);
                connFlag = true;
                Intent intent = new Intent("com.sq.wolf.mqtt");
                intent.putExtra("msg", CONNECTED);
                sendBroadcast(intent);
            } catch (MqttException e) {
                e.printStackTrace();
                Intent intent = new Intent("com.sq.wolf.mqtt");
                intent.putExtra("msg", FAIL);
                sendBroadcast(intent);
            }
        }

        public void publish(String topic, MqttMessage msg) {
            try {
                mqttClient.publish(topic, msg);
            } catch (MqttException e) {
                e.printStackTrace();
            }
        }

        public void disconnect() {
            if (connFlag) {
                try {
                    mqttClient.disconnect();
                } catch (MqttException e) {
                    e.printStackTrace();
                }
                connFlag = false;
            }
        }

        public boolean getConnStatus() {
            return connFlag;
        }
    }
    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return binder;
    }

    @Override
    public void onCreate() {
        super.onCreate();

    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }
}
