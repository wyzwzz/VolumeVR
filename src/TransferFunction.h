//
// Created by vr on 2020/11/30.
//

#ifndef OPENVRSDK_TRANSFERFUNCTION_H
#define OPENVRSDK_TRANSFERFUNCTION_H
#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#define TF_DIM 256
using namespace glm;
using namespace std;
class TransferFunction{
private:
    float* transferFunction= nullptr;
    float* preIntTransferFunction= nullptr;
    map<glm::uint8,glm::vec4> transferFunctionSettings;//key is 0-255 and value RGBA is float within 0.0-1.0
    //用于计算预积分传输函数的参数
    int baseSampleNumber=20;
    int ratio=1;
public:
    TransferFunction()=default;
    ~TransferFunction(){
        delete[] transferFunction;
        delete[] preIntTransferFunction;
    }

    //从json文件中读取传输函数设置，暂未实现
    void loadTransferFunctionInfo(const string& path);

    void setTransferFunctionInfo(map<glm::uint8,glm::vec4> settings);
    void setTransferFunctionInfo(map<float,glm::vec4> settings);
    void setPreIntTransferFunctionArgs(int baseSampleNumber,int ratio){
        this->baseSampleNumber=baseSampleNumber;
        this->ratio=ratio;
    }

    void generateTransferFunction(bool reGenerate=false);
    void calPreIntTransferFunction(bool reCalculate=false);

    float* getTransferFunction();
    float* getPreIntTransferFunction();

    void printTransferFunction(){
        for(int i=0;i<TF_DIM;i++)
            printf("%d: %.2f %.2f %.2f %.5f\n",i,transferFunction[4*i+0],transferFunction[4*i+1],transferFunction[4*i+2],transferFunction[4*i+3]);
    }
    void printPreIntTransferFunction(){
        for(int i=0;i<TF_DIM;i++)
            for(int j=0;j<=i;j++)
                cout<<i<<" "<<j<<": "<<preIntTransferFunction[4*(i*TF_DIM+j)+3]<<"\n";
    }
};

inline void TransferFunction::loadTransferFunctionInfo(const string& path)
{
//    Json::Reader reader;
//    Json::Value root;
//
//    ifstream in(path,ios::binary);
//    if(!in.is_open()){
//        cout<<"File open failed!"<<endl;
//        return;
//    }
//    if(reader.parse(in,root)){
//
//    }
//    else{
//        cout<<"ERROR parse!"<<endl;
//    }
}

inline void TransferFunction::generateTransferFunction(bool reGenerate)
{
    if(transferFunctionSettings.size()==0){
        cout<<"Transfer function settings' info has not loaded!\n";
        return;
    }
    if(transferFunction!= nullptr && reGenerate==false){
        cout<<"ERROR!Transfer function exists!\n";
        return;
    }
    if(transferFunction!=nullptr && reGenerate==true){
        cout<<"Re-generate transfer function!\n";
    }
    else if(transferFunction== nullptr){
        cout<<"Initial calculate transfer function!\n";
    }
    transferFunction=new float[4*TF_DIM];
    if(transferFunction!= nullptr){
        cout<<"Memory alloc successfully!\n";
    }
    else{
        cout<<"Memory alloc failed!\n";
        return;
    }

    assert(TF_DIM==256);
    vector<glm::uint8> keys;
    for(auto it=transferFunctionSettings.begin();it!=transferFunctionSettings.end();it++)
        keys.push_back(it->first);
    int size=keys.size();
    assert(size);
    cout<<"Transfer function key point size is: "<<size<<"\n";
    //如果两边的端点不是0与255,那么小于最小值和大于最大值的部分分别等于最小点和最大点的数值
    for(int i=0;i<keys[0];i++){
        transferFunction[i*4+0]=transferFunctionSettings[keys[0]].r;
        transferFunction[i*4+1]=transferFunctionSettings[keys[0]].g;
        transferFunction[i*4+2]=transferFunctionSettings[keys[0]].b;
        transferFunction[i*4+3]=transferFunctionSettings[keys[0]].a;
    }
    for(int i=keys[size-1];i<TF_DIM;i++){
        transferFunction[i*4+0]=transferFunctionSettings[keys[size-1]].r;
        transferFunction[i*4+1]=transferFunctionSettings[keys[size-1]].g;
        transferFunction[i*4+2]=transferFunctionSettings[keys[size-1]].b;
        transferFunction[i*4+3]=transferFunctionSettings[keys[size-1]].a;
    }

    for(int i=1;i<size;i++){
        int left=keys[i-1],right=keys[i];
        vec4 leftColor=transferFunctionSettings[left];
        vec4 rightColor=transferFunctionSettings[right];
        for(int j=left;j<=right;j++){
            transferFunction[j*4+0]=1.0f*(j-left)/(right-left)*rightColor[0]+1.0*(right-j)/(right-left)*leftColor[0];
            transferFunction[j*4+1]=1.0f*(j-left)/(right-left)*rightColor[1]+1.0*(right-j)/(right-left)*leftColor[1];
            transferFunction[j*4+2]=1.0f*(j-left)/(right-left)*rightColor[2]+1.0*(right-j)/(right-left)*leftColor[2];
            transferFunction[j*4+3]=1.0f*(j-left)/(right-left)*rightColor[3]+1.0*(right-j)/(right-left)*leftColor[3];
        }
    }
}

inline void TransferFunction::calPreIntTransferFunction(bool reCalculate)
{
    assert(TF_DIM==256);
    if(transferFunction==nullptr){
        cout<<"ERROR! Transfer function is unloaded!\n";
        return;
    }
    if(preIntTransferFunction!= nullptr && reCalculate==false){
        cout<<"Pre-calculate-interpret transfer function exist\n";
        return;
    }
    else if(preIntTransferFunction!= nullptr && reCalculate==true){
        cout<<"Re-calculate pre-interpret transfer function!\n";
        delete[] preIntTransferFunction;
    }
    else if(preIntTransferFunction== nullptr){
        cout<<"Initial calculate pre-interpret transfer function!\n";
    }
    preIntTransferFunction=new float[4*TF_DIM*TF_DIM];
    if(preIntTransferFunction== nullptr){
        cout<<"Memory alloc failed\n";
        return;
    }
    else{
        cout<<"Memory alloc successfully\n";
    }

    float rayStep=1.0;
    assert(baseSampleNumber>0);
    assert(ratio>0);
    clock_t time=clock();
    for(int sb=0;sb<256;sb++){
        for(int sf=0;sf<=sb;sf++){
            int offset=sf!=sb;
            int n=baseSampleNumber+ratio*abs(sb-sf);
            float stepWidth=rayStep/n;
            float rgba[4]={0,0,0,0};
            for(int i=0;i<n;i++){
                float s=sf+(sb-sf)*(float)i / n;
                float sFrac=s-floor(s);
                float opacity=(transferFunction[int(s)*4+3]*(1.0-sFrac)+transferFunction[((int)s+offset)*4+3]*sFrac)*stepWidth;
                float temp=exp(-rgba[3])*opacity;
                rgba[0]+=(transferFunction[(int)s*4+0]*(1.0-sFrac)+transferFunction[(int(s)+offset)*4+0]*sFrac)*temp;
                rgba[1]+=(transferFunction[(int)s*4+1]*(1.0-sFrac)+transferFunction[(int(s)+offset)*4+1]*sFrac)*temp;
                rgba[2]+=(transferFunction[(int)s*4+2]*(1.0-sFrac)+transferFunction[(int(s)+offset)*4+2]*sFrac)*temp;
                rgba[3]+=opacity;
            }
            preIntTransferFunction[(sf*TF_DIM+sb)*4+0]=preIntTransferFunction[(sb*TF_DIM+sf)*4+0]=rgba[0];
            preIntTransferFunction[(sf*TF_DIM+sb)*4+1]=preIntTransferFunction[(sb*TF_DIM+sf)*4+1]=rgba[1];
            preIntTransferFunction[(sf*TF_DIM+sb)*4+2]=preIntTransferFunction[(sb*TF_DIM+sf)*4+2]=rgba[2];
            preIntTransferFunction[(sf*TF_DIM+sb)*4+3]=preIntTransferFunction[(sb*TF_DIM+sf)*4+3]=1.0-exp(-rgba[3]);
        }
    }
    cout<<"Calculate pre-InterpretTF time is : "<<(clock()-time)/(double)CLOCKS_PER_SEC<<"sec\n";
}

inline float *TransferFunction::getTransferFunction() {
    if(transferFunction== nullptr){
        cout<<"ERROR! Transfer function is empty!\n";
    }
    return transferFunction;
}

inline float *TransferFunction::getPreIntTransferFunction() {
    if(preIntTransferFunction== nullptr){
        cout<<"ERROR! Pre-calculate-interpret transfer function is empty!\n";
    }
    return preIntTransferFunction;
}

inline void TransferFunction::setTransferFunctionInfo(map<glm::uint8,glm::vec4> settings)
{
    this->transferFunctionSettings=settings;
}
inline void TransferFunction::setTransferFunctionInfo(map<float,glm::vec4> settings)
{
    this->transferFunctionSettings.clear();
    for(auto it=settings.begin();it!=settings.end();it++){
        glm::uint8 idx=255*it->first;
        this->transferFunctionSettings[idx]=it->second;
    }
}

#endif //OPENVRSDK_TRANSFERFUNCTION_H
