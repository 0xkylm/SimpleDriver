int prot_offset = 0x879;

struct _PS_PROTECTION {
    union
    {
        UCHAR Level;                                                        //0x0
        union
        {
            UCHAR Type : 3;                                                   //0x0
            UCHAR Audit : 1;                                                  //0x0
            UCHAR Signer : 4;                                                 //0x0
        };
    };
};

struct protect {
	UCHAR SectionSignatureLevel;                                            //0x879
	_PS_PROTECTION Protection;                                       //0x87a

};