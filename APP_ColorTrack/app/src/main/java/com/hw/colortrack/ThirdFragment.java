package com.hw.colortrack;

import android.os.*;
import android.app.Activity;
import android.support.v4.app.Fragment;
import android.util.*;
import android.text.*;
import android.graphics.*;
import android.view.View.*;
import android.view.*;
import android.widget.TextView.*;
import android.widget.*;
import android.widget.LinearLayout.LayoutParams;
import android.graphics.drawable.*;
import android.widget.AdapterView.*;
import android.content.Context;

import java.util.*;
import java.util.concurrent.*;
import java.text.*;
import android.content.*;
import android.transition.*;

public class ThirdFragment extends Fragment {
    private View view;
    private MainActivity mActivity;
    private Client client;

    private Toast toast;

    private Button monitorBtn;
    private TextView timeTv;
    private TextView frameTv;
    private TextView anglehTv, anglevTv;
    private TextView trackTv;
    private LinearLayout trackLinear;
    private TextView xposTv, yposTv, widthTv, heightTv;

    private Handler mHandler;
    private Handler fHandler = new Handler() {

        @Override
        public void handleMessage(Message msg) {
            // TODO: Implement this method
            switch (msg.what) {
                case 1:
                    if (!client.monitorFlag) {
                        monitorBtn.setText("开始监测");
                    } else {
                        monitorBtn.setText("停止监测");
                    }
                    break;
                case 15:
                    timeTv.setText("当前时间 " + client.getDetailTime());
                    break;
                case 21:
                    if (!client.monitorFlag) {
                        monitorBtn.setText("开始监测");
                    } else {
                        monitorBtn.setText("停止监测");
                    }
                    toast.setText("设置成功");
                    toast.show();
                    break;
                case 22:
                    frameTv.setText("帧率:" + client.frame + " fps");
                    anglehTv.setText("水平角:" + client.angleh + "°");
                    anglevTv.setText("垂直角:" + client.anglev + "°");


                    if (!client.findFlag) {
                        trackTv.setVisibility(0);//visible
                        trackLinear.setVisibility(8);//gone
                    } else {
                        trackTv.setVisibility(8);
                        trackLinear.setVisibility(0);
                        xposTv.setText("x坐标:" + client.xpos);
                        yposTv.setText("y坐标:" + client.ypos);
                        widthTv.setText("宽度:" + client.width);
                        heightTv.setText("高度:" + client.height);
                    }
                    break;
                default:
                    break;
            }
            super.handleMessage(msg);
        }

    };


    @Override
    public void onAttach(Activity activity) {
        // TODO: Implement this method
        super.onAttach(activity);
        mActivity = (MainActivity)activity;
        client = mActivity.getClient();
        mHandler = mActivity.getHandler();
        client.setHandler(fHandler, 2);
    }


    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        view = inflater.inflate(R.layout.third_tab, container, false);
        return view;
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        // TODO: Implement this method
        super.onActivityCreated(savedInstanceState);
        toast = Toast.makeText(getActivity(), "", Toast.LENGTH_SHORT);
        initView();

    }

    private void initView() {

        timeTv = (TextView)view.findViewById(R.id.time_tv);
        new Timer().start();

        monitorBtn = (Button)view.findViewById(R.id.monitor_btn);

        Message msg = new Message();
        msg.what = 1;
        fHandler.sendMessage(msg);

        monitorBtn.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                if (!client.monitorFlag) {
                    client.sendCmd(Command.start_monitor);
                } else {
                    client.sendCmd(Command.stop_monitor);
                }

            }
        });

        frameTv = (TextView)view.findViewById(R.id.frame_tv);
        anglehTv = (TextView)view.findViewById(R.id.angleh_tv);
        anglevTv = (TextView)view.findViewById(R.id.anglev_tv);
        trackTv = (TextView)view.findViewById(R.id.track_tv);
        trackLinear = (LinearLayout)view.findViewById(R.id.track_linear);

        xposTv = (TextView)view.findViewById(R.id.xpos_tv);
        yposTv = (TextView)view.findViewById(R.id.ypos_tv);

        widthTv = (TextView)view.findViewById(R.id.width_tv);
        heightTv = (TextView)view.findViewById(R.id.height_tv);
    }

    private class Timer extends Thread {

        @Override
        public void run() {
            while (true) {
                try {
                    Message msg = new Message();
                    msg.what = 15;
                    fHandler.sendMessage(msg);
                    Thread.sleep(1000);
                } catch (Exception e) {
                    continue;
                }
            }
        }
    }


}
