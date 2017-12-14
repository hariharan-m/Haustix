package com.hariharanm.haustix;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;
import android.os.Handler;
import android.widget.ToggleButton;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Set;
import java.util.UUID;

public class MainActivity extends Activity {
    //    private final String DEVICE_NAME="H-C-2010-06-01(3366)";
    private final String DEVICE_ADDRESS="00:03:19:50:2B:3B";
    private final UUID PORT_UUID = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb");//Serial Port Service ID
    private BluetoothDevice device;
    private BluetoothSocket socket;
    private OutputStream outputStream;
    private InputStream inputStream;
    TextView pir1,pir2,lightlevel;
    Switch lr1,blinds,l0,l1,l2,l3;
    ToggleButton btConnect, toggleAuto;
    Button btnRefresh;
    boolean deviceConnected=false;
    Thread thread;
    byte buffer[];
    int bufferPosition;
    boolean stopThread;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        btConnect = (ToggleButton) findViewById(R.id.tbConnect);
        btConnect.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (buttonView.isChecked()){
                    if(BTinit())
                    {
                        if(BTconnect())
                        {
                            setUiEnabled(true);
                            deviceConnected=true;
                            beginListenForData();
                        }

                    }
                }
                else
                {
                    stopThread = true;
                    try {
                        outputStream.close();
                        inputStream.close();
                        socket.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    setUiEnabled(false);
                    deviceConnected=false;
                }
            }

        });
        lr1 = (Switch) findViewById(R.id.swr1l);
        lr1.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                // TODO Auto-generated method stub
                if (buttonView.isChecked()){
                    BTSend("s101");
                }
                else
                {
                    BTSend("s100");
                }
            }

    });
        l0 = (Switch) findViewById(R.id.sw0);
        l0.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                // TODO Auto-generated method stub
                if (buttonView.isChecked()){
                    BTSend("s201");
                }
                else
                {
                    BTSend("s200");
                }
            }

        });
        l1 = (Switch) findViewById(R.id.sw1);
        l1.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                // TODO Auto-generated method stub
                if (buttonView.isChecked()){
                    BTSend("s211");
                }
                else
                {
                    BTSend("s210");
                }
            }

        });
        l2 = (Switch) findViewById(R.id.sw2);
        l2.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                // TODO Auto-generated method stub
                if (buttonView.isChecked()){
                    BTSend("s221");
                }
                else
                {
                    BTSend("s220");
                }
            }

        });

        l3 = (Switch) findViewById(R.id.sw3);
        l3.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                // TODO Auto-generated method stub
                if (buttonView.isChecked()){
                    BTSend("s231");
                }
                else
                {
                    BTSend("s230");
                }
            }

        });
        blinds = (Switch) findViewById(R.id.swBlinds);
        blinds.setEnabled(false);
        blinds.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                // TODO Auto-generated method stub
                if (buttonView.isChecked()){
                    BTSend("s111");
                }
                else
                {
                    BTSend("s112");
                }
            }

        });

        toggleAuto = (ToggleButton) findViewById(R.id.tbAuto);
        toggleAuto.setChecked(true);
        toggleAuto.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                // TODO Auto-generated method stub
                if (buttonView.isChecked()){
                    BTSend("s12");
                    blinds.setEnabled(false);
                }
                else
                {
                    blinds.setEnabled(true);
                }
            }

        });
        btnRefresh = (Button) findViewById(R.id.btnRefresh);
        pir1 = (TextView) findViewById(R.id.tvPir1);
        pir2 = (TextView) findViewById(R.id.tvPir2);
        lightlevel = (TextView) findViewById(R.id.tvLuxLevel);
    setUiEnabled(false);

    }

    public void setUiEnabled(boolean bool)
    {

        lr1.setEnabled(bool);
        blinds.setEnabled(bool);
        l0.setEnabled(bool);
        l1.setEnabled(bool);
        l2.setEnabled(bool);
        l3.setEnabled(bool);
        btnRefresh.setEnabled(bool);
        toggleAuto.setEnabled(bool);

    }

    public boolean BTinit()
    {
        boolean found=false;
        BluetoothAdapter bluetoothAdapter=BluetoothAdapter.getDefaultAdapter();
        if (bluetoothAdapter == null) {
            Toast.makeText(getApplicationContext(),"Device doesnt Support Bluetooth",Toast.LENGTH_SHORT).show();
        }
        if(!bluetoothAdapter.isEnabled())
        {
            Intent enableAdapter = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableAdapter, 0);
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        Set<BluetoothDevice> bondedDevices = bluetoothAdapter.getBondedDevices();
        if(bondedDevices.isEmpty())
        {
            Toast.makeText(getApplicationContext(),"Please Pair the Device first",Toast.LENGTH_SHORT).show();
        }
        else
        {
            for (BluetoothDevice iterator : bondedDevices)
            {
                if(iterator.getAddress().equals(DEVICE_ADDRESS))
                {
                    device=iterator;
                    found=true;
                    break;
                }
            }
        }
        return found;
    }

    public boolean BTconnect()
    {
        boolean connected=true;
        try {
            socket = device.createRfcommSocketToServiceRecord(PORT_UUID);
            socket.connect();
        } catch (IOException e) {
            e.printStackTrace();
            connected=false;
        }
        if(connected)
        {
            try {
                outputStream=socket.getOutputStream();
            } catch (IOException e) {
                e.printStackTrace();
            }
            try {
                inputStream=socket.getInputStream();
            } catch (IOException e) {
                e.printStackTrace();
            }

        }


        return connected;
    }

    public void onClickStart(View view) {
        if(BTinit())
        {
            if(BTconnect())
            {
                setUiEnabled(true);
                deviceConnected=true;
                beginListenForData();
            }

        }
    }

    void beginListenForData()
    {
        final Handler handler = new Handler();
        stopThread = false;
        buffer = new byte[1024];
        Thread thread  = new Thread(new Runnable()
        {
            public void run()
            {
                while(!Thread.currentThread().isInterrupted() && !stopThread)
                {
                    try
                    {
                        int byteCount = inputStream.available();
                        if(byteCount > 0)
                        {
                            byte[] rawBytes = new byte[byteCount];
                            inputStream.read(rawBytes);
                            final String string=new String(rawBytes,"UTF-8");
                            handler.post(new Runnable() {
                                public void run()
                                {
                                char ldr = string.charAt(2);
                                char p1 = string.charAt(4);
                                char p2 = string.charAt(9);
                                if(ldr=='L') {
                                    lightlevel.setText("Low light");

                                }
                                else if(ldr=='M'){
                                    lightlevel.setText("Normal light");

                                }
                                else if(ldr=='H')
                                {
                                    lightlevel.setText("Normal light");

                                }
                                if(p1=='0')
                                {
                                    pir1.setText("Room Empty");

                                }
                                else if(p1=='1')
                                {
                                    pir1.setText("Room Occupied");

                                }
                                if(p2=='0')
                                {
                                    pir2.setText("Room Empty");

                                }
                                else if(p2=='1')
                                {
                                    pir2.setText("Room Occupied");

                                }

                                }
                            });

                        }
                    }
                    catch (IOException ex)
                    {
                        stopThread = true;
                    }
                }
            }
        });

        thread.start();
    }

    public void BTSend(String string) {
        try {
            outputStream.write(string.getBytes());
        } catch (IOException e) {
            e.printStackTrace();
        }
    }



    public void onClickRefresh(View view) {
        BTSend("g");
    }
}
