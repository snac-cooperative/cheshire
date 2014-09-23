/*
 *  Copyright (c) 1990-2012 [see Other Notes, below]. The Regents of the
 *  University of California (Regents). All Rights Reserved.
 *  
 *  Permission to use, copy, modify, and distribute this software and its
 *  documentation for educational, research, and not-for-profit purposes,
 *  without fee and without a signed licensing agreement, is hereby
 *  granted, provided that the above copyright notice, this paragraph and
 *  the following two paragraphs appear in all copies, modifications, and
 *  distributions. Contact The Office of Technology Licensing, UC
 *  Berkeley, 2150 Shattuck Avenue, Suite 510, Berkeley, CA 94720-1620,
 *  (510) 643-7201, for commercial licensing opportunities. 
 *  
 *  Created by Ray R. Larson 
 *             School of Information Management and Systems, 
 *             University of California, Berkeley.
 *  
 *    
 *       IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT,
 *       INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,
 *       INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE
 *       AND ITS DOCUMENTATION, EVEN IF REGENTS HAS BEEN ADVISED OF THE
 *       POSSIBILITY OF SUCH DAMAGE. 
 *    
 *       REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT
 *       NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 *       FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE AND
 *       ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS
 *       PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE
 *       MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
 */
/************************************************************************
*
*	Header Name:	in_compress_utils.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	utility routines for applying various compressed 
*                       encodings for bitmaps.
*
*	Usage:		called from index creation and compression code
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		Nov. 9, 2012
*	Revised:	
*	Version:	1.0
*	Copyright (c) 2012.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#define MAIN
#include "cheshire.h"
#include "configfiles.h"
#include "bitmaps.h"
#include "compress_dat.h"

/* global config file structure */
config_file_info *cf_info_base;

/* the following is needed for searches of allied indexes */
char *LastResultSetID;
int current_user_id;

int last_rec_proc = 0;
 
extern idx_list_entry *cf_getidx_entry(char *filename, char *indexname);


extern int create_indexes(idx_list_entry *idx, int *num_open, idx_list_entry *head) ;
extern idx_list_entry *cf_getidx_entry(char *filename, char *indexname);
  
static cdatmap_t *c_block_ptr


int IntReader(c_block *source)
{
    char c;
    bool eof;


    int getInt()
    {
        int temp=0;
        bool untouched = true;
        int i=0;
        while(i<3 && f . get ( c ) && !(f . eof()))
        {
            untouched=false;
            temp|=c;
            temp<<=sizeof(char)*8;
            i++;
        }
        if(i<3){

            //not enought char to fill in int
            temp>>=sizeof(char)*8;
        }
        if(f . get ( c ) && !(f . eof()))
        {
            untouched=false;
            temp|=c;
        }
        //cout << "un: " << untouched << endl;
        if(untouched == true){
            eof=true;
        }
        return temp;
    }
    bool hasLeft()
    {
        if(eof==true){
            return false;
        }else{
            return !(f . eof());
        }
    }
    void close()
    {
        f . close ();
    }
};
class IntWriter
{
public:
    ofstream f;
    IntWriter(const char * dest)
    {
        f.open ( dest, ios::binary | ios::out );
    }
    void putInt(int put)
    {
        //cout << "put " << put << endl;
        char chrs[4];
        int i=0;
        while(i<4){
            chrs[i]=0;
            i++;
        }
        chrs[3]|=put;
        put>>=sizeof(char)*8;
        chrs[2]|=put;
        put>>=sizeof(char)*8;
        chrs[1]|=put;
        put>>=sizeof(char)*8;
        chrs[0]|=put;
        i=0;
        while(i<4){
            f.put (chrs[i]);
            i++;
        }
        //cout << endl;
    }
    void close(){
        f.close();
    }
};
class BitReader
{
public:
    ifstream      f;
    bool eof;
    char c;
    const char * i_source;
    unsigned char mask;
    BitReader(const char * source)
    {
        eof=false;
        c=0;
        mask=128;
        f.open ( source, ios::binary | ios::in );
        i_source=source;
        f.get(c);
    }
    bool getBit()
    {
        if(mask==0)
        {
            if(f.get(c) && !(f.eof())){
                mask=128;
            }else{
                eof=true;
            }
        }
        if ((c & mask)>0)
        {
            mask>>=1;
            return true;
        }
        else if ((c & mask)==0)
        {
            mask>>=1;
            return false;
        }
        return true;
    }
    bool hasLeft()
    {
        if(eof==true){
            return false;
        }else{
            return !(f . eof());
        }
    }
    void close()
    {
        f . close ();
    }
};
class BitWriter
{
public:
    const char * i_destination;
    ofstream f;
    unsigned char writer;
    unsigned char mask;
    BitWriter(const char * dest)
    {
        f.open ( dest, ios::binary | ios::out );
        i_destination=dest;
        writer=0;
        mask=128;
    }
    void putBit(bool toPut)
    {
        if(mask==0)
        {
            f.put(writer);
            writer=0;
            mask=128;
        }
        if(toPut==true)
        {
            writer=writer | mask;
        }/*else if(toPut==false){
        }*/
        mask>>=1;
    }
    void close()
    {
        if(mask>0 && writer>0){
            f.put(writer);
        }
        f . close ();
    }
};
class BitStack{
    public:
    int * stacker;
    int mask;
    unsigned int len;
    int size;
    BitStack(){
        mask=1;
        size=1;
        stacker = new int[size];
        stacker[size-1]=0;
        len=0;
    }
    void pushBit(int input){
        stacker[size-1]<<=1;
        //cout << "push " << input << " stacker " << stacker[size-1] << endl;
        stacker[size-1]=stacker[size-1]|input;
        len++;
        if(len==8*sizeof(int)*size){
            /*
                need new size of stack
            */
            //cout << "zvetsil" << endl;
            size++;
            int * tmp = new int[size-1];
            int i=0;
            while(i<(size-1)){
                tmp[i]=stacker[i];
                i++;
            }
            delete [] stacker;
            stacker = new int[size];
            i=0;
            while(i<(size-1)){
                stacker[i]=tmp[i];
                i++;
            }
            stacker[size-1]=0;
        }
    }
    int length(){
        //cout << len << endl;
        return len;
    }
    bool popBit(){
        //cout << "mask " << tmp << " pop " << (stacker&tmp) << " stacker " << stacker << endl;
        int to_ret=stacker[size-1];
        stacker[size-1]>>=1;
        len--;
        //cout << " len: " << len << " sizeint: " << 8*(size-1)*sizeof(int) ;
        if(len==(8*(size-1)*sizeof(int)) && len>0){
            //cout << "zmensil" << endl;
            //zmenseni zasobniku
            int * tmp = new int[size-1];
            int i=0;
            while(i<(size-1)){
                tmp[i]=stacker[i];
                i++;
            }
            delete [] stacker;
            stacker = new int[size-1];
            i=0;
            while(i<(size-1)){
                stacker[i]=tmp[i];
                i++;
            }
            size--;
        }
        return to_ret&mask;
    }
};


bool toElias  ( const char * srcName, const char * dstName )
{
    if(!(fexists(srcName)))
    {
        return false;
    }
    IntReader intreader(srcName);
    BitWriter bitwriter(dstName);
    while (intreader.hasLeft())
    {
        int num = intreader.getInt()+1;
        BitStack bits;
        while (num > 1) {
            int len = 0;
            for (int temp = num; temp > 0; temp >>= 1)  // calculate 1+floor(log2(num))
                len++;
            for (int i = 0; i < len; i++)
                bits.pushBit((num >> i) & 1);
            num = len - 1;
        }
        while (bits.length() > 0)
            bitwriter.putBit(bits.popBit());
        bitwriter.putBit(false);                        // write one zero
    }
    bitwriter.close();
    intreader.close();
    return true;
}

bool toBinary ( const char * srcName, const char * dstName )
{
    if(!(fexists(srcName)))
    {
        return false;
    }
    BitReader bitreader(srcName);
    IntWriter intwriter(dstName);
    while (bitreader.hasLeft())
    {
        int num = 1;
        //bitReader.inputBit()...???
        while (bitreader.getBit() && bitreader.hasLeft())     // potentially dangerous with malformed files.
        {
            int len = num;
            num = 1;
            for (int i = 0; i < len; ++i)
            {
                num <<= 1;
                if (bitreader.getBit())
                    num |= 1;
            }
        }
        intwriter.putInt(num-1);
    }
    bitreader.close();
    intwriter.close();
    return true;
}
#ifndef __PROGTEST__
int main ( int argc, char * argv [] )
{


    /*BitReader intreader("pokus1.bin");
    BitWriter intwriter("pokus2.bin");
    while(intreader.hasLeft()){
        cout << intreader.hasLeft() << endl;
        int myint = intreader.getBit();
            intwriter.putBit(myint);
    }
    */
    toElias("echo","pokus2.bin");
    toBinary("pokus2.bin","echoun");

    return 1;
}
#endif /* __PROGTEST__ */




 void golombEncode(char* source, char* dest, int M)
 {
     IntReader intreader(source);
     BitWriter bitwriter(dest);
     while(intreader.hasLeft())
     {
         int num = intreader.getInt();
         int q = num / M;
         for (int i = 0 ; i < q; i++)
             bitwriter.putBit(true);   // write q ones
         bitwriter.putBit(false);      // write one zero
         int v = 1;
         for (int i = 0 ; i < log2(M); i++)
         {            
             bitwriter.putBit( v & num );  
             v = v << 1;         
         }
     }
     bitwriter.close();
     intreader.close();
 }

void golombDecode(char* source, char* dest, int M)
 {
     BitReader bitreader(source);
     IntWriter intwriter(dest);
     int q = 0;
     int nr = 0;
     while (bitreader.hasLeft())
     {
         nr = 0;
         q = 0;
         while (bitreader.getBit()) q++;     // potentially dangerous with malformed files.
         for (int a = 0; a < log2(M); a++)   // read out the sequential log2(M) bits
             if (bitreader.getBit())
                 nr += 1 << a;
         nr += q*M;                          // add the bits and the multiple of M
         intwriter.putInt(nr);               // write out the value
     }
     bitreader.close();
     intwriter.close();
 }

void eliasGammaEncode(char* source, char* dest)
{
    IntReader intreader(source);
    BitWriter bitwriter(dest);
    while (intreader.hasLeft())     
    {
        int num = intreader.getInt();
        int l = log2(num);
        for (int a=0; a < l; a++)
            bitwriter.putBit(false); //put 0s to indicate how many bits will follow
        bitwriter.putBit(true);      //mark the end of the 0s
        for (int a=l-1; a >= 0; a--) //Write the bits as plain binary
        {
            if (num & 1 << a)
                bitwriter.putBit(true);
            else
                bitwriter.putBit(false);
        }
    }
    intreader.close();
    bitwriter.close();
}


void eliasGammaDecode(char* source, char* dest)
{
    BitReader bitreader(source);
    IntWriter intwriter(dest);
    while (bitreader.hasLeft())
    {
        int numberBits = 0;
        while (!bitreader.getBit() && bitreader.hasLeft())
            numberBits++; //keep on reading until we fetch a one...
        int current = 0;
        for (int a=numberBits-1; a >= 0; a--) //Read numberBits bits
        {
            if (bitreader.getBit())
                current |= 1 << a;
        }
        current |= 1 << numberBits; //leading 1 isn't encoded!
 
        intwriter.putInt(current);
    }
}
