/**
* Read mp4 file,decode and save each frame as YUV picture
*/
#include<stdio.h>
#include<stdlib.h>
#include<libavcodec/avcodec.h>

#define INBUF_SIZE 8192


int main(int argc, char **argv){

	const char *mp4File,*yuvFile;
	AVCodec *codec;
    AVCodecContext *codecContext;
	AVFrame *frame;
    AVPacket packet;
	AVCodecParserContext *pCodecParserCtx=NULL;

	int first_time=1;

	if(argc<=2){
		printf("need more argument!");
		return -1;
	}
	mp4File=argv[1];
	yuvFile=argv[2];
	printf("Demo for decode %s to %s",mp4File,yuvFile);

	avcodec_register_all();
  
 	/* find the MPEG-4 video decoder */
    codec = avcodec_find_decoder(AV_CODEC_ID_MPEG4);
	 if (!codec) {
        printf("Codec not found\n");
        exit(1);
    }

	codecContext= avcodec_alloc_context3(codec);
	if(!codecContext){
		printf("CodecContext not alloc\n");
	}
	
	pCodecParserCtx=av_parser_init(AV_CODEC_ID_MPEG4);
	if (avcodec_open2(codecContext, codec, NULL) < 0) {  
        printf("Could not open codec\n");  
        return -1;  
    }  
    uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE]={0};
	
	FILE * infile=fopen(mp4File,"rb");
    FILE * fp_out=fopen(yuvFile,"wb");
	int cur_size=0;
	uint8_t * cur_ptr;
	av_init_packet(&packet);
    frame=av_frame_alloc();

	while(1){
		cur_size = fread(inbuf, 1, INBUF_SIZE, infile);  
    	if (cur_size == 0)  
            break;  
        cur_ptr=inbuf;  
  
        while (cur_size>0){  
            int len = av_parser_parse2(  
            pCodecParserCtx, codecContext,  
            &packet.data, &packet.size,  
            cur_ptr , cur_size ,  
            AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);  
  
            cur_ptr += len;  
            cur_size -= len;  
  
            if(packet.size==0)  
                continue;  
  
            //Some Info from AVCodecParserContext  
            printf("[Packet]Size:%6d\t",packet.size);  
            switch(pCodecParserCtx->pict_type){  
                case AV_PICTURE_TYPE_I: printf("Type:I\t");break;  
                case AV_PICTURE_TYPE_P: printf("Type:P\t");break;  
                case AV_PICTURE_TYPE_B: printf("Type:B\t");break;  
                default: printf("Type:Other\t");break;  
            }  
            printf("Number:%4d\n",pCodecParserCtx->output_picture_number);  
			int got_picture=0;
			int ret = avcodec_decode_video2(codecContext, frame, &got_picture, &packet); 
			if(ret<0){
			 printf("decode packet error");  
			}else{
				if(got_picture){
   					if(first_time){  
                   	 printf("\nCodec Full Name:%s\n",codecContext->codec->long_name);  
                   	 printf("width:%d\nheight:%d\n\n",codecContext->width,codecContext->height);  
                     first_time=0;  
                }  
                //Y, U, V  
				int i=0;
                for(i=0;i<frame->height;i++){  
                    fwrite(frame->data[0]+frame->linesize[0]*i,1,frame->width,fp_out);  
                }  
                for( i=0;i<frame->height/2;i++){  
                    fwrite(frame->data[1]+frame->linesize[1]*i,1,frame->width/2,fp_out);  
                }  
                for( i=0;i<frame->height/2;i++){  
                    fwrite(frame->data[2]+frame->linesize[2]*i,1,frame->width/2,fp_out);  
                }  
  
                printf("Succeed to decode 1 frame! height=%d,width=%d\n",frame->height,frame->width);  
			
			}
			}
		}
  
	}

	
	

    return 0;
	
}











































