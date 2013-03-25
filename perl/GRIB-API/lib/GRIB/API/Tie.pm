package GRIB::API::Tie;
use Carp;

sub TIEHASH {
	my ($class,$handle) = @_;
	bless({handle=>$handle},$class);
}

sub FETCH {
	my ($self,$key) = @_;
	return $self->{handle}->get($key);
}

sub STORE {
	my ($self,$key,$value) = @_;
	$self->{handle}->set($key,$value) or warn "Cannot set $key to $value";
	delete $self->{keys};
}

sub EXISTS {
	my ($self,$key) = @_;
	die;
	return $self->{handle}->get_size($key);
}

sub CLEAR {
	my ($self) = @_;
	warn "GRIB::API::Tie::CLEAR ignored";

}

sub FIRSTKEY {
	my ($self) = @_;
	$self->{keys} = $self->{handle}->get_keys();
	$self->{n} = 0;
	$self->{keys}->[$self->{n}++];
}

sub NEXTKEY {
	my ($self,$last) = @_;
	croak "Cannot iterate and modify GRIB at the same time"
		unless(exists $self->{keys});
	$self->{keys}->[$self->{n}++];
}

1;
