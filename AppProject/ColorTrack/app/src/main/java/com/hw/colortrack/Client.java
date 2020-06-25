package com.hw.colortrack;

import java.io.*;
import java.net.*;
import java.util.*;
import java.util.concurrent.*;
import java.text.*;

import android.content.*;
import android.os.*;
import android.bluetooth.*;
import android.util.Log;
import android.util.*;
import android.graphics.Color;

enum Command {
    start, stop, up, down, left, right, setsize, pick,
    setcolor, defcolor, start_monitor, stop_monitor,
    camera_reset, reset, exit, init, message
}


public class Client {

    private BluetoothSocket socket;
    private BluetoothDevice device;
    private final String BD_ADDR = "00:18:E4:36:1E:80";
    public boolean isConnected;

    public SocketThread st;

    public boolean isStarted, isFreeDefined;
    public boolean monitorFlag, findFlag;
    public String frame;
    public String width, height, xpos, ypos;
    public String angleh, anglev;
    public int colorIdx;
    public int[] setRgb = {0, 0, 0xff};
    public int[] setHsl = {0, 0, 0};
    final int colorDefined[] = {0xff0000, 0x00ff00, 0x0000ff, 0xffff00};


    private Handler mHandler;
    private Handler[] handler = new Handler[3];
    private Message msg;


    public void connect() {
        try {
            BleConfig();
            UUID uuid = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
            socket = device.createRfcommSocketToServiceRecord(uuid);
            socket.connect();
            isConnected = true;
            st = new SocketThread(socket);
            st.start();
        } catch (Exception e) {
            isConnected = false;
        }
    }

    private void BleConfig() {
        BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
        if (!adapter.isEnabled()) {
            adapter.enable();
            adapter.cancelDiscovery();
        }
        while (!adapter.startDiscovery()) {
            try {
                Thread.sleep(100);
            } catch (Exception e)
            {}
        }
        device = adapter.getRemoteDevice(BD_ADDR);
    }

    public void setMainHandler(Handler mHandler) {
        this.mHandler = mHandler;

    }

    public void setHandler(Handler handler, int idx) {
        this.handler[idx] = handler;
    }

    public int[] rgbHex2intarr(int rgbHex) {
        int[] rgb = new int[3];
        rgb[0] = (rgbHex & 0xff0000) >> 16;
        rgb[1] = (rgbHex & 0x00ff00) >> 8;
        rgb[2] = rgbHex & 0xff;
        return rgb;
    }



    public int rgbHex2color(int rgbHex) {
        int[] rgb = rgbHex2intarr(rgbHex);
        return Color.rgb(rgb[0], rgb[1], rgb[2]);
    }

    public int getColorDefined() {
        return rgbHex2color(colorDefined[colorIdx]);

    }

    public int getColorFreeDefined() {
        return Color.rgb(setRgb[0], setRgb[1], setRgb[2]);
    }

    public static float RGBFromHue(float a, float b, float h) {
        if (h < 0) {
            h += 360;
        }
        if (h >= 360) {
            h -= 360;
        }
        if (h < 60) {
            return a + ((b - a) * h) / 60;
        }
        if (h < 180) {
            return b;
        }
        if (h < 240) {
            return a + ((b - a) * (240 - h)) / 60;
        }
        return a;
    }

    private int[] hsl2rgb(int[] hsl) {
        int[] rgb = new int[3];

        float H = hsl[0];
        float S = hsl[1];
        float L = hsl[2];

        float R, G, B, var_1, var_2;
        if (S == 0) {
            R = L;
            G = L;
            B = L;
        } else {
            if (L < 128) {
                var_2 = (L * (256 + S)) / 256;
            } else {
                var_2 = (L + S) - (S * L) / 256;
            }

            if (var_2 > 255) {
                var_2 = Math.round(var_2);
            }

            if (var_2 > 254) {
                var_2 = 255;
            }

            var_1 = 2 * L - var_2;
            R = RGBFromHue(var_1, var_2, H + 120);
            G = RGBFromHue(var_1, var_2, H);
            B = RGBFromHue(var_1, var_2, H - 120);
        }
        R = R < 0 ? 0 : R;
        R = R > 255 ? 255 : R;
        G = G < 0 ? 0 : G;
        G = G > 255 ? 255 : G;
        B = B < 0 ? 0 : B;
        B = B > 255 ? 255 : B;
        rgb[0] = Math.round(R);
        rgb[1] = Math.round(G);
        rgb[2] = Math.round(B);

        return rgb;
    }

    private void sendMsg(int idx, int label) {
        msg = new Message();
        msg.what = label;
        if (handler[idx] != null) {
            handler[idx].sendMessage(msg);
        }
    }

    public void sendCmd(Command cmd, byte... args) {
        byte[] buf = new byte[args.length + 3];
        buf[0] = (byte)0xfe;
        buf[1] = (byte)buf.length;
        buf[2] = (byte)cmd.ordinal();
        for (int i = 0; i < args.length; i++) {
            buf[3 + i] = args[i];
        }

        st.sendBuf(buf);
    }

    public String getTime() {
        return new SimpleDateFormat("yyyy-MM-dd HH:mm:ss").format(new Date());
    }

    public String getDetailTime() {
        return new SimpleDateFormat("HH:mm:ss").format(new Date());
    }


    private class SocketThread extends Thread {

        private BluetoothSocket socket;
        private DataInputStream in;
        private DataOutputStream out;
        private BufferedReader br;


        public SocketThread(BluetoothSocket socket) throws IOException {
            this.socket = socket;
            in = new DataInputStream(socket.getInputStream());
            out = new DataOutputStream(socket.getOutputStream());
            //pw = new PrintWriter(socket.getOutputStream());
            br = new BufferedReader(new InputStreamReader(socket.getInputStream()));

        }


        @Override
        public void run() {
            fg:
            while (true) {
                try {
                    String buf = br.readLine();
                    String[] data = buf.split(" ");
                    int cmd = Integer.parseInt(data[0]);


                    if (cmd == Command.start.ordinal()) {
                        isStarted = true;
                        sendMsg(0, 21);

                    } else if (cmd == Command.stop.ordinal()) {
                        isStarted = false;
                        sendMsg(0, 21);
                    } else if (cmd >= Command.up.ordinal() && cmd <= Command.right.ordinal()) {

                        sendMsg(0, 22);
                    } else if (cmd == Command.setsize.ordinal()) {

                        sendMsg(0, 23);
                    } else if (cmd == Command.pick.ordinal()) {
                        isFreeDefined = true;
                        setHsl[0] = Integer.parseInt(data[1]);
                        setHsl[1] = Integer.parseInt(data[2]);
                        setHsl[2] = Integer.parseInt(data[3]);

                        setRgb = hsl2rgb(setHsl);

                        sendMsg(0, 24);
                        sendMsg(1, 21);
                    }

                    else if (cmd == Command.setcolor.ordinal()) {
                        isFreeDefined = false;

                        colorIdx = Integer.parseInt(data[1]);

                        //setRgb=rgbHex2intarr(colorDefined[colorIdx]);

                        sendMsg(1, 22);
                    } else if (cmd == Command.defcolor.ordinal()) {
                        isFreeDefined = true;
                        setRgb[0] = Integer.parseInt(data[1]);
                        setRgb[1] = Integer.parseInt(data[2]);
                        setRgb[2] = Integer.parseInt(data[3]);

                        sendMsg(1, 23);
                    }

                    else if (cmd == Command.start_monitor.ordinal()) {
                        monitorFlag = true;
                        sendMsg(2, 21);
                    } else if (cmd == Command.stop_monitor.ordinal()) {
                        monitorFlag = false;
                        sendMsg(2, 21);
                    } else if (cmd == Command.message.ordinal()) {
                        frame = data[1];
                        angleh = data[2];
                        anglev = data[3];
                        if (data[4].equals("1")) {
                            findFlag = true;
                            xpos = data[5];
                            ypos = data[6];
                            width = data[7];
                            height = data[8];
                        } else {
                            findFlag = false;
                        }

                        sendMsg(2, 22);
                    } else if (cmd == Command.init.ordinal()) {
                        if (data[1].equals("1")) {
                            isStarted = true;
                        } else {
                            isStarted = false;
                        }
                        if (data[2].equals("1")) {
                            monitorFlag = true;
                        } else {
                            monitorFlag = false;
                        }
                        if (data[3].equals("0")) {
                            isFreeDefined = false;
                            colorIdx = Integer.parseInt(data[4]);
                        } else {
                            isFreeDefined = true;
                            setHsl[0] = Integer.parseInt(data[4]);
                            setHsl[1] = Integer.parseInt(data[5]);
                            setHsl[2] = Integer.parseInt(data[6]);
                            setRgb = hsl2rgb(setHsl);
                        }
                    }

                    else if (cmd == Command.camera_reset.ordinal()) {
                        msg = new Message();
                        msg.what = 21;
                        mHandler.sendMessage(msg);

                    } else if (cmd == Command.reset.ordinal()) {
                        isStarted = false;
                        isFreeDefined = false;
                        monitorFlag = false;
                        colorIdx = Integer.parseInt(data[1]);

                        msg = new Message();
                        msg.what = 22;
                        mHandler.sendMessage(msg);

                        sendMsg(0, 1);
                        sendMsg(1, 1);
                        sendMsg(2, 1);

                    }

                    else if (cmd == Command.exit.ordinal()) {
                        msg = new Message();
                        msg.what = 0;

                        break fg;
                    }

                } catch (Exception e) {
                    msg = new Message();
                    msg.what = 1;

                    break fg;
                }
            }
            try {
                out.close();
                in.close();
                socket.close();
                socket = null;
                mHandler.sendMessage(msg);
            } catch (IOException ex)
            {}
        }


        public void sendBuf(byte[] buf) {
            try {
                out.write(buf);
                out.flush();
            } catch (Exception e) {
                Log.e("exp", "Write Failed!");
            }

        }


    }
}


