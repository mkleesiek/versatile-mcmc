/**
 * @file io.h
 *
 * @date 24.08.2016
 * @author marco@kleesiek.com
 * @description
 */

#ifndef VMCMC_IO_H_
#define VMCMC_IO_H_

#include <vmcmc/sample.h>

#include <string>
#include <fstream>

namespace vmcmc
{

class Writer
{
public:
    Writer() { };
    virtual ~Writer() { };

    virtual Writer* Clone() const = 0;

    virtual bool Initialize(size_t chainIndex, size_t totalNChains) = 0;
    virtual void Write(const Sample& sample) = 0;

    void Write(const Chain& chain);

protected:
    size_t fCurrentIndex = 0;
};

class AsciiWriter : public Writer
{
public:
    AsciiWriter(const std::string& directory, const std::string& stem,
        const std::string& separator = "-", const std::string& extension = ".txt");
    virtual ~AsciiWriter();

    virtual AsciiWriter* Clone() const override;

    void SetPrecision(int prec) { fPrecision = prec; }

    virtual bool Initialize(size_t chainIndex, size_t totalNChains) override;
    virtual void Write(const Sample& sample) override;

private:
    AsciiWriter(const AsciiWriter& other);

    std::string fDirectory;
    std::string fStem;
    std::string fSeparator;
    std::string fExtension;

    int fPrecision = 12;

    std::ofstream fFileStream;
};

inline void Writer::Write(const Chain& chain)
{
    for (; fCurrentIndex < chain.size(); fCurrentIndex++)
        Write( chain[fCurrentIndex] );
}

} /* namespace vmcmc */

#endif /* VMCMC_IO_H_ */
