
$dir = "." unless length($dir = $ARGV[0]);

$IN_METHOD = 0;
$IN_DESCRIPTION = 0;
$IN_ATTRIBUTE = 0;

@CLASSES;
$this_dir;
print "<?xml version=\"1.0\"?>\n";
print "<?xml-stylesheet href=\"iabc_autodoc.xsl\" type=\"text/xsl\"?>\n";
print "<PROJECT>\n";
ProcessDir($dir);
print "</PROJECT>\n";

sub ProcessDir
   {
   my ($dir) = @_;
   my @dirs;
   if (-d $dir)
      {
      chdir($dir);
      $this_dir = $_;
      @dirs = &GetDir;
      foreach (@dirs)
         {
         next if (/^\./);
         ProcessDir($_);
         }
         chdir("..") unless $dir =~ /^\./;
      }
   else
      {
      &ProcessFile($dir);
      }
   }

sub PopClass
{
   $class = pop(CLASSES);
   if (length($class))
   {
      print "</CLASS>\n";
   }
}

sub PushClass
{
   ($class_name) = @_;
   push (CLASSES,$class_name);
   print "<CLASS value=\"$class_name\">\n";
}

sub ProcessFile
{
   my ($file) = @_;
   if (($file =~ /\.hh/i) || ($file =~ /\.h$/i) || ($file =~ /\.cpp$/i))
      {
         print "<FILE value=\"$file\" dir=\"$this_dir\">\n";
         open(IN,$file);
         while (<IN>)
         {
             if (/\/\/ *CLASS:\s*(\S+)/)
             {
                $mt = Unescape($1);
                EndDescription();
                EndMethod();
                EndAttribute();
                PopClass;
                PushClass($mt);
             }
             elsif (/\/\/ *METHOD:\s*(\S+)/)
             {
                $mt = Unescape($1);
                EndDescription();
                EndAttribute();
                EndMethod();
                $IN_METHOD = 1;
                print "<METHOD value=\"$mt\">\n";
             }
             elsif (/\/\/ *ATTRIBUTE:\s*(\S+)/)
             {
                $mt = Unescape($1);
                EndDescription();
                EndAttribute();
                EndMethod();
                $IN_ATTRIBUTE = 1;
                print "<ATTRIBUTE value=\"$mt\">\n";
             }
             elsif (/\/\/ *DESCRIPTION:/)
             {
                EndDescription();
                print "<DESCRIPTION>\n";
                $IN_DESCRIPTION = 1;
             }
             elsif ($IN_DESCRIPTION)
             {
                if (/ *\/\/(.+)/)
                {
                   $mt = Unescape($1);
                   print "$mt\n";
                }
                else
                {
                   EndDescription();
                   EndAttribute();
                   EndMethod();
                }
             }
         }
         EndDescription();
         EndAttribute();
         EndMethod();
         PopClass;
         print "</FILE>\n";
      }
}

sub Unescape
{
   ($foo) =@_;
   $foo =~ s/&/ &amp\; /g;
   $foo =~ s/</ &lt\; /g;
   $foo =~ s/>/ &gt\; /g;
   $foo;
}

sub EndAttribute
{
   if ($IN_ATTRIBUTE)
   {
      $IN_ATTRIBUTE = 0;
      print "</ATTRIBUTE>\n";
   }
}

sub EndDescription
{
   if ($IN_DESCRIPTION)
   {
      $IN_DESCRIPTION = 0;
      print "</DESCRIPTION>\n";
   }
}

sub EndMethod
{
   if ($IN_METHOD)
   {
      $IN_METHOD = 0;
      print "</METHOD>\n";
   }
}

sub GetDir
   {
   opendir(DIR,".");
   @rv = readdir(DIR);
   closedir(DIR);
   @rv
   }




