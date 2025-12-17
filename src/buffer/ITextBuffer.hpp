class ITextBuffer {
    public :
        virtual ~ITextBuffer() {};
        virtual void insert(char character) = 0;
};